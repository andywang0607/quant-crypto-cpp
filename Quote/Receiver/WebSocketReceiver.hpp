#ifndef __WEBSOCKETRECEIVER_H__
#define __WEBSOCKETRECEIVER_H__

#include <thread>

#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>
#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_client.hpp>

namespace QuantCrypto::Quote {

using client = websocketpp::client<websocketpp::config::asio_tls_client>;
using context_ptr = std::shared_ptr<boost::asio::ssl::context>;
using message_ptr = websocketpp::config::asio_client::message_type::ptr;

template <typename HandlerType>
class WebSocketReceiver
{
public:
    explicit WebSocketReceiver(const nlohmann::json &config)
        : handler_(config)
    {
        using websocketpp::lib::bind;
        using websocketpp::lib::placeholders::_1;
        using websocketpp::lib::placeholders::_2;

        // set logging policy if needed
        client_.clear_access_channels(websocketpp::log::alevel::frame_header);
        client_.clear_access_channels(websocketpp::log::alevel::frame_payload);

        // Initialize ASIO
        client_.init_asio();
        client_.set_tls_init_handler(bind(&WebSocketReceiver::on_tls_init));

        // Register our handlers
        client_.set_open_handler(bind(&WebSocketReceiver::onOpen, this, _1));
        client_.set_close_handler(bind(&WebSocketReceiver::onClose, this, _1));
        client_.set_fail_handler(bind(&WebSocketReceiver::onFail, this, _1));
        client_.set_message_handler(bind(&WebSocketReceiver::onMessage, this, _1, _2));
    }

    ~WebSocketReceiver()
    {
        close(websocketpp::close::status::going_away);
        if (wsThread_.joinable()) {
            wsThread_.join();
        }
    }

    bool connect()
    {
        try {
            // Create a connection to the given URI and queue it for connection once
            // the event loop starts
            websocketpp::lib::error_code ec;
            client::connection_ptr con = client_.get_connection(HandlerType::Uri, ec);
            spdlog::info("[WebSocketReceiver] connect uri: {}", HandlerType::Uri);
            if (ec) {
                spdlog::info("[WebSocketReceiver] could not create connection: {}", ec.message());
                return false;
            }

            hdl_ = con->get_handle();
            client_.connect(con);
            wsThread_ = std::thread(&client::run, &client_);

            spdlog::info("[WebSocketReceiver] connect: {}", ec.message());
        } catch (const std::exception &e) {
            spdlog::error("[WebSocketReceiver] exception: {}", e.what());
        } catch (websocketpp::lib::error_code e) {
            spdlog::error("[WebSocketReceiver] exception: {}", e.message());
        } catch (...) {
            spdlog::error("[WebSocketReceiver] other exception");
        }

        return true;
    }

private:
    void onOpen(websocketpp::connection_hdl)
    {
        spdlog::info("[WebSocketReceiver] onOpen");
        subscribe();
    }

    void onFail(websocketpp::connection_hdl)
    {
        spdlog::warn("[WebSocketReceiver] onFail: Connection Failed");
    }

    void onMessage(websocketpp::connection_hdl, message_ptr msg)
    {
        if (msg->get_opcode() != websocketpp::frame::opcode::text) {
            spdlog::warn("[WebSocketReceiver] unsupported opcode: {}", msg->get_opcode());
        }

        handler_.onMessage(msg->get_payload());
    }

    void onClose(websocketpp::connection_hdl)
    {
        client::connection_ptr con = client_.get_con_from_hdl(hdl_);

        spdlog::info("[WebSocketReceiver] onClose: Connection Closed, remote close code: {}", con->get_remote_close_code());
    }

    void subscribe()
    {
        auto subscribeMsgs = handler_.genSubscribeMsg();
        for (const auto &req : subscribeMsgs) {
            websocketpp::lib::error_code ec;

            client_.send(hdl_, req.dump(), websocketpp::frame::opcode::text, ec);
            if (ec) {
                spdlog::error("[WebSocketReceiver] Error sending message: {}", ec.message());
                return;
            }
            spdlog::info("[WebSocketReceiver] subscribe Message: {}", req.dump());
        }
    }

    static context_ptr on_tls_init()
    {
        // establishes a SSL connection
        context_ptr ctx = std::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::sslv23);

        try {
            ctx->set_options(boost::asio::ssl::context::default_workarounds |
                             boost::asio::ssl::context::no_sslv2 |
                             boost::asio::ssl::context::no_sslv3 |
                             boost::asio::ssl::context::single_dh_use);
        } catch (std::exception &e) {
            spdlog::info("[BybitQuote] Error in context pointer: {}", e.what());
        }
        return ctx;
    }


    client client_;
    websocketpp::connection_hdl hdl_;
    std::thread wsThread_;
    HandlerType handler_;
};
} // namespace QuantCrypto::Quote
#endif // __WEBSOCKETRECEIVER_H__
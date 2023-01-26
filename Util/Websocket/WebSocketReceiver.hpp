#ifndef __WEBSOCKETRECEIVER_H__
#define __WEBSOCKETRECEIVER_H__

#include "Logger.hpp"
#include "Timer.hpp"

#include <chrono>
#include <thread>

#include <nlohmann/json.hpp>
#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_client.hpp>

namespace Util::Websocket {

using client = websocketpp::client<websocketpp::config::asio_tls_client>;
using context_ptr = std::shared_ptr<boost::asio::ssl::context>;
using message_ptr = websocketpp::config::asio_client::message_type::ptr;

template <typename HandlerType>
class WebSocketReceiver
{
public:
    explicit WebSocketReceiver(const nlohmann::json &config)
        : handler_(config)
        , logger_("WebSocketReceiver")
        , isOpen_(false)
        , timer_("WSTimer")
    {
        using websocketpp::lib::bind;
        using websocketpp::lib::placeholders::_1;
        using websocketpp::lib::placeholders::_2;

        if (handler_.enabled()) {
            // set logging policy if needed
            client_.clear_access_channels(websocketpp::log::alevel::frame_header);
            client_.clear_access_channels(websocketpp::log::alevel::frame_payload);

            // Initialize ASIO
            client_.init_asio();
            client_.set_tls_init_handler(bind(&WebSocketReceiver::on_tls_init, this));

            // Register our handlers
            client_.set_open_handler(bind(&WebSocketReceiver::onOpen, this, _1));
            client_.set_close_handler(bind(&WebSocketReceiver::onClose, this, _1));
            client_.set_fail_handler(bind(&WebSocketReceiver::onFail, this, _1));
            client_.set_message_handler(bind(&WebSocketReceiver::onMessage, this, _1, _2));
            client_.set_ping_handler(bind(&WebSocketReceiver::onPing, this, _1, _2));

            // ping pong timer
            if constexpr (HandlerType::needPeriodicPing()) {
                timer_.periodic([this]() {
                    if (!isOpen_) {
                        return;
                    }
                    websocketpp::lib::error_code ec;
                    const auto pingMsg = handler_.genPingMessage();
                    client_.send(hdl_, pingMsg.dump(), websocketpp::frame::opcode::text, ec);
                    if (ec) {
                        logger_.error("Error ping message: {}", ec.message());
                        return;
                    }
                    logger_.info("ping Message: {}", pingMsg.dump());
                },
                                30 * 1000 * 1000);
            }

            connect();
        }
    }

    ~WebSocketReceiver()
    {
        timer_.stop();
        close(websocketpp::close::status::going_away);
        for (auto &thraed : wsThread_) {
            if (thraed.joinable()) {
                thraed.join();
            }
        }
        wsThread_.clear();
    }

    inline HandlerType &getHandler()
    {
        return handler_;
    }

private:
    bool connect()
    {
        try {
            // Create a connection to the given URI and queue it for connection once
            // the event loop starts
            websocketpp::lib::error_code ec;
            client::connection_ptr con = client_.get_connection(HandlerType::Uri, ec);
            logger_.info("connect uri: {}", HandlerType::Uri);
            if (ec) {
                logger_.error("could not create connection: {}", ec.message());
                return false;
            }

            hdl_ = con->get_handle();
            client_.connect(con);
            wsThread_.emplace_back(&client::run, &client_);

            logger_.info("connect: {}", ec.message());
        } catch (const std::exception &e) {
            logger_.error("exception: {}", e.what());
        } catch (websocketpp::lib::error_code e) {
            logger_.error("exception: {}", e.message());
        } catch (...) {
            logger_.error("other exception");
        }

        return true;
    }

    void onOpen(websocketpp::connection_hdl)
    {
        logger_.info("onOpen");
        isOpen_ = true;
        subscribe();
        timer_.start();
    }

    void onFail(websocketpp::connection_hdl)
    {
        logger_.warn("onFail: Connection Failed");
    }

    bool onPing(websocketpp::connection_hdl, std::string payload)
    {
        logger_.info("onPing, payload={}", payload);
        return true;
    }

    void onMessage(websocketpp::connection_hdl, message_ptr msg)
    {
        if (msg->get_opcode() != websocketpp::frame::opcode::text) {
            logger_.warn("unsupported opcode: {}", msg->get_opcode());
        }

        handler_.onMessage(msg->get_payload());
    }

    void onClose(websocketpp::connection_hdl)
    {
        client::connection_ptr con = client_.get_con_from_hdl(hdl_);
        isOpen_ = false;
        logger_.warn("onClose: Connection Closed, remote close code={}, reconnect ret={}", con->get_remote_close_code(), connect());
    }

    void subscribe()
    {
        auto subscribeMsgs = handler_.genSubscribeMsg();
        for (const auto &req : subscribeMsgs) {
            websocketpp::lib::error_code ec;

            client_.send(hdl_, req.dump(), websocketpp::frame::opcode::text, ec);
            if (ec) {
                logger_.error("Error sending message: {}", ec.message());
                return;
            }
            logger_.info("subscribe Message: {}", req.dump());
        }
    }

    context_ptr on_tls_init()
    {
        // establishes a SSL connection
        context_ptr ctx = std::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::sslv23);

        try {
            ctx->set_options(boost::asio::ssl::context::default_workarounds |
                             boost::asio::ssl::context::no_sslv2 |
                             boost::asio::ssl::context::no_sslv3 |
                             boost::asio::ssl::context::single_dh_use);
        } catch (std::exception &e) {
            logger_.error("Error in context pointer: {}", e.what());
        }
        return ctx;
    }


    client client_;
    websocketpp::connection_hdl hdl_;
    std::vector<std::thread> wsThread_;
    Util::Thread::Timer timer_;
    
    HandlerType handler_;
    Util::Log::Logger logger_;
    bool isOpen_;
};
} // namespace Util-Websocket
#endif // __WEBSOCKETRECEIVER_H__
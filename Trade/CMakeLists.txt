set(targets TradeApi BybitTradeAdapter)
foreach(target ${targets})
  add_subdirectory(${target})
endforeach(target)

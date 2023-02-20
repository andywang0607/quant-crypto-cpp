set(targets TradeNode BybitTradeAdapter)
foreach(target ${targets})
  add_subdirectory(${target})
endforeach(target)

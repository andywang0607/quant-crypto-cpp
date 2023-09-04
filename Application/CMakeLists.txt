set(targets QuoteReceiver QuantCrypto)
foreach(target ${targets})
  add_subdirectory(${target})
endforeach(target)

#!/usr/bin/env ruby
require 'bundler'
Bundler.setup
require 'ffi-rzmq'

begin
  ctx = ZMQ::Context.new
  socket = ctx.socket ZMQ::SUB
  puts "Connecting to locator at #{ARGV[0]}"
  rc = socket.connect( ARGV[0] )
  raise "Connect failed" if rc != 0
  rc = socket.setsockopt( ZMQ::SUBSCRIBE, "" ) 
  raise "Set socket options failed" if rc != 0
  message = ""
  count = 0
  while count < 3
    rc = socket.recv_string( message, ZMQ::DONTWAIT )
    break if rc >= 0
    sleep 1
    count += 1
  end
  #raise "Failed to receive expected messages error #{ZMQ::Util.error_string}" if rc < 0
  puts "Heartbeat => #{ message }"
  
rescue => e
  puts "script failed #{e.message}"
  puts e.backtrace.join( "\n" )
  exit 1
end

exit 0


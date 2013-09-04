#!/usr/bin/env ruby
require 'bundler'
Bundler.setup
require 'ffi-rzmq'

begin
  ctx = ZMQ::Context.new
  socket = ctx.socket ZMQ::PULL
  puts "Binding to port  #{ARGV[0]}"
  rc = socket.bind( "tcp://*:#{ARGV[0]}" )
  raise "Connect failed" if rc != 0

  message = ""
  count = 0
  while true
    rc = socket.recv_string( message )
    #break if rc >= 0
    puts message
  end

  #raise "Failed to receive expected messages error #{ZMQ::Util.error_string}" if rc < 0
  puts "Message => #{ message }"
  puts "-----------------------------------------------"
rescue => e
  puts "script failed #{e.message}"
  puts e.backtrace.join( "\n" )
  exit 1
end

exit 0


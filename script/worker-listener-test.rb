#!/usr/bin/env ruby
require 'bundler'
Bundler.setup

$: << File.join( File.dirname( __FILE__ ), '..', 'lib' ) 
$: << File.join( File.dirname( __FILE__ ), '..', '.' )


#require 'init-daemon'
require 'ffi-rzmq'

begin
  ctx = ZMQ::Context.new(1)
#  wl = InitDaemon::WorkerListener.new( ctx,  "tcp://*:5602", "A" )
#  wl.listen

  socket = ctx.socket ZMQ::PULL 

  rc = socket.bind "tcp://*:8000" 
  puts "bind returns #{rc}"
  message = ""
  while true
    rc = socket.recv_string( message )

    if rc == 0 
      puts message
    else
      puts "recv returned error #{ZMQ::Util.error_string }"
      break
    end

  end
ctx.terminate

rescue => e
  print e.backtrace.join("\n")
end



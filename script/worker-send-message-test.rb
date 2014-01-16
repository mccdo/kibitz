#!/usr/bin/env ruby
require 'bundler'
Bundler.setup

$: << File.join( File.dirname( __FILE__ ), '..', 'lib' ) 
$: << File.join( File.dirname( __FILE__ ), '..', '.' )


#require 'init-daemon'
require 'ffi-rzmq'
require 'json'

begin

  ctx = ZMQ::Context.new(1)
  sock = ctx.socket( ZMQ::PUSH )
  rc = sock.connect( "tcp://localhost:8000" )
  sleep 2
  puts "connected return #{rc}"

  while true
    puts "sending"
    msg = {"message_type"=>"notification","notification_type"=>"worker_status_message","status"=>3,"timestamp"=>"2013-09-18T01:42:36.400316","version"=>"1.0","worker_id"=>"1","worker_type"=>"A"}.to_json
    rc = sock.send_string msg
    puts "return code is #{rc}"
    o
  end

rescue => e
  print e.backtrace.join("\n")
end



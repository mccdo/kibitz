#!/usr/bin/perl

use strict;
use warnings;
use 5.10.0;

use ZMQ::LibZMQ3;
use ZMQ::Constants qw( ZMQ_SUB ZMQ_SUBSCRIBE ); 

my $ctx = zmq_init();
die( "zmq_init failed" ) if( !$ctx );

my $socket = zmq_socket( $ctx, ZMQ_SUB );
die( "zmq_socket failed" ) if( !$socket );

my $rc = zmq_connect( $socket, $ARGV[ 0 ] );
die( "Connect failed" ) if( $rc != 0 );

$rc = zmq_setsockopt( $socket, ZMQ_SUBSCRIBE, "" ); 
die( "Set socket options failed" ) if( $rc != 0 );

my $msg = zmq_recvmsg( $socket, 0 );
my $str = zmq_msg_data( $msg );
say( $str );

$rc = zmq_msg_close( $msg );
$rc = zmq_close( $socket );
$rc = zmq_term( $ctx );


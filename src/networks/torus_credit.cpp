// $Id$

/*
 Copyright (c) 2007-2015, Trustees of The Leland Stanford Junior University
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 Redistributions of source code must retain the above copyright notice, this 
 list of conditions and the following disclaimer.
 Redistributions in binary form must reproduce the above copyright notice, this
 list of conditions and the following disclaimer in the documentation and/or
 other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*torus_credit.cpp
 *
 * Unidirectional torus topology with credit-based flow control
 * Each dimension has only one direction (clockwise)
 */

#include "booksim.hpp"
#include <vector>
#include <sstream>
#include <cassert>
#include "torus_credit.hpp"
#include "routefunc.hpp"
#include "random_utils.hpp"
#include "misc_utils.hpp"

TorusCredit::TorusCredit( const Configuration &config, const string & name ) :
Network( config, name )
{
  _ComputeSize( config );
  _Alloc( );
  _BuildNet( config );
}

void TorusCredit::_ComputeSize( const Configuration &config )
{
  _k = config.GetInt( "k" );
  _n = config.GetInt( "n" );

  gK = _k; gN = _n;
  _size     = powi( _k, _n );
  
  // Only one channel per dimension (unidirectional)
  _channels = _n * _size;

  _nodes = _size;
}

void TorusCredit::RegisterRoutingFunctions() {
  // Leave empty like KNCube - routing functions are registered globally
}

void TorusCredit::_BuildNet( const Configuration &config )
{
  int input_channel;
  int output_channel;

  ostringstream router_name;

  // Latency type, noc or conventional network
  bool use_noc_latency;
  use_noc_latency = (config.GetInt("use_noc_latency")==1);
  
  for ( int node = 0; node < _size; ++node ) {

    router_name << "router";
    
    if ( _k > 1 ) {
      for ( int dim_offset = _size / _k; dim_offset >= 1; dim_offset /= _k ) {
        router_name << "_" << ( node / dim_offset ) % _k;
      }
    }

    // Each router has _n input channels + 1 injection channel
    // and _n output channels + 1 ejection channel
    _routers[node] = Router::NewRouter( config, this, router_name.str( ), 
                                        node, _n + 1, _n + 1 );
    _timed_modules.push_back(_routers[node]);

    router_name.str("");

    for ( int dim = 0; dim < _n; ++dim ) {

      // Get the output channel number for this node
      output_channel = _OutputChannel( node, dim );
      
      // Get the input channel number (from previous node in ring)
      int prev_node = _PrevNode( node, dim );
      input_channel = _OutputChannel( prev_node, dim );

      // Add the input channel (from previous node in the ring)
      _routers[node]->AddInputChannel( _chan[input_channel], _chan_cred[input_channel] );

      // Add the output channel (to next node in the ring)
      _routers[node]->AddOutputChannel( _chan[output_channel], _chan_cred[output_channel] );

      // Torus channel is longer due to wrap around
      int latency = 2; // torus latency

      // Set channel latency
      if(use_noc_latency){
        _chan[output_channel]->SetLatency( latency );
        _chan_cred[output_channel]->SetLatency( latency );
      } else {
        _chan[output_channel]->SetLatency( 1 );
        _chan_cred[output_channel]->SetLatency( 1 );
      }
    }
    
    // Injection and ejection channels, always 1 latency
    _routers[node]->AddInputChannel( _inject[node], _inject_cred[node] );
    _routers[node]->AddOutputChannel( _eject[node], _eject_cred[node] );
    _inject[node]->SetLatency( 1 );
    _eject[node]->SetLatency( 1 );
  }
}

int TorusCredit::_OutputChannel( int node, int dim )
{
  // Each node has _n output channels numbered sequentially
  return _n * node + dim;
}

int TorusCredit::_NextNode( int node, int dim )
{
  int k_to_dim = powi( _k, dim );
  int loc_in_dim = ( node / k_to_dim ) % _k;
  int next_node;
  
  // Always go clockwise (positive direction)
  if ( loc_in_dim == ( _k-1 ) ) {
    // Wrap around to beginning of dimension
    next_node = node - (_k-1)*k_to_dim;
  } else {
    // Move to next position in dimension
    next_node = node + k_to_dim;
  }

  return next_node;
}

int TorusCredit::_PrevNode( int node, int dim )
{
  int k_to_dim = powi( _k, dim );
  int loc_in_dim = ( node / k_to_dim ) % _k;
  int prev_node;
  
  // Previous node in clockwise direction
  if ( loc_in_dim == 0 ) {
    // Wrap around to end of dimension
    prev_node = node + (_k-1)*k_to_dim;
  } else {
    // Move to previous position in dimension
    prev_node = node - k_to_dim;
  }

  return prev_node;
}

int TorusCredit::GetN( ) const
{
  return _n;
}

int TorusCredit::GetK( ) const
{
  return _k;
}

double TorusCredit::Capacity( ) const
{
  // Capacity is halved compared to bidirectional torus
  return (double)_k / 8.0;
}
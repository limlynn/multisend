#ifndef PROCESS_HPP
#define PROCESS_HPP

#include <vector>
#include <functional>
#include <limits.h>

class Process
{
public:
  class SampledFunction {
  private:
    const double _offset;
    const double _bin_width;
    std::vector< double > _function;

    unsigned int to_bin( double x ) const { int ret = (x - _offset) / _bin_width; if ( ret < 0 ) { return 0; } else if ( ret >= (int)_function.size() ) { return _function.size() - 1; } else { return ret; } }

    double from_bin_floor( unsigned int bin ) const { if ( bin <= 0 ) { return -INT_MAX; } else { return bin * _bin_width + _offset; } }
    double from_bin_ceil( unsigned int bin ) const { if ( bin >= _function.size() - 1 ) { return INT_MAX; } else { return (bin + 1) * _bin_width + _offset; } }
    double from_bin_mid( unsigned int bin ) const { return (bin + 0.5) * _bin_width + _offset; }

  public:
    SampledFunction( const int num_samples, const double maximum_value, const double minimum_value );

    double & operator[]( const double x ) { return _function[ to_bin( x ) ]; }

    double sample_floor( double x ) const { return from_bin_floor( to_bin( x ) ); }
    double sample_ceil( double x ) const { return from_bin_ceil( to_bin( x ) ); }

    void for_each( const std::function< void( const double midpoint, double & value ) > f );
    void for_each( const std::function< void( const double midpoint, const double & value ) > f ) const;
    void for_range( const double min, const double max,
		    const std::function< void( const double midpoint, double & value ) > f );

    const SampledFunction & operator=( const SampledFunction & other );
  };

private:
  SampledFunction _probability_mass_function;

  const double _brownian_motion_rate; /* stddev of difference after one second */

public:
  Process( const int maximum_rate, const double s_brownian_motion_rate );

  /* apply brownian motion */
  void evolve( const double time );

  /* update from new observation */
  void observe( const double time, const int counts );

  /* make pmf sum to unity */
  void normalize( void );

  const SampledFunction & pmf( void ) { return _probability_mass_function; }
};

#endif
#pragma once

#include <cstdint>
#include <functional>

#include "L1_Peripheral/interrupt.hpp"
#include "L1_Peripheral/lpc40xx/pin.hpp"
#include "utility/status.hpp"

namespace sjsu
{
/// An abstract interface for General Purpose I/O
/// @ingroup l1_peripheral
class Gpio
{
 public:
  // ===========================================================================
  // Interface Defintions
  // ===========================================================================
  /// Defines the set of directions a GPIO can be.
  enum Direction : uint8_t
  {
    kInput  = 0,
    kOutput = 1
  };

  /// Defines what states a GPIO pin can be in.
  enum State : uint8_t
  {
    kLow  = 0,
    kHigh = 1
  };

  /// Defines the set of events that can trigger a GPIO interrupt.
  enum class Edge : uint8_t
  {
    kRising  = 0,
    kFalling = 1,
    kBoth    = 2
  };

  // ===========================================================================
  // Interface Methods
  // ===========================================================================

  /// Set pin as an output or an input
  ///
  /// NOTE: this method acts is the GPIO initialization, and must be called
  ///       first before calling any other method
  ///
  /// @param direction - which direction to set the pin to.
  virtual void SetDirection(Direction direction) const = 0;

  /// Set the pin state as HIGH voltage or LOW voltage
  virtual void Set(State output) const = 0;

  /// Toggles pin state. If the pin is HIGH, after this call it will be LOW
  /// and vise versa.
  virtual void Toggle() const = 0;

  /// @return the state of the pin, note that this method does not consider
  ///         whether or not the active level is high or low. Simply returns the
  ///         state as depicted in memory
  virtual bool Read() const = 0;

  /// @return underlying pin object
  virtual const sjsu::Pin & GetPin() const = 0;

  /// Attach an interrupt call to a pin
  ///
  /// @param callback - the callback supplied here will be executed when the
  ///        interrupt condition occurs
  /// @param edge - the pin condition that will trigger the interrupt
  virtual void AttachInterrupt(InterruptCallback callback, Edge edge) = 0;

  /// Remove interrupt call from pin and deactivate interrupts for this pin
  virtual void DetachInterrupt() const = 0;

  // ===========================================================================
  // Utility Methods
  // ===========================================================================

  /// Set pin to HIGH voltage
  void SetHigh() const
  {
    Set(State::kHigh);
  }

  /// Set pin to LOW voltage
  void SetLow() const
  {
    Set(State::kLow);
  }

  /// Set pin direction as input
  void SetAsInput() const
  {
    SetDirection(Direction::kInput);
  }

  /// Set pin direction as output
  void SetAsOutput() const
  {
    SetDirection(Direction::kOutput);
  }

  /// Set pin to run callback when the pin sees a rising edge
  ///
  /// @param callback - the function to be called when a rising edge event
  ///                   occurs on this pin.
  void OnRisingEdge(InterruptCallback callback)
  {
    AttachInterrupt(callback, Edge::kRising);
  }

  /// Set pin to run callback when the pin sees a falling edge
  ///
  /// @param callback - the function to be called when a falling edge event
  ///                   occurs on this pin.
  void OnFallingEdge(InterruptCallback callback)
  {
    AttachInterrupt(callback, Edge::kFalling);
  }

  /// Set pin to run callback when the pin sees a change on the pin's state.
  ///
  /// @param callback - the function to be called when a rising edge or falling
  ///                   edge event occurs on this pin.
  void OnChange(InterruptCallback callback)
  {
    AttachInterrupt(callback, Edge::kBoth);
  }
};
}  // namespace sjsu

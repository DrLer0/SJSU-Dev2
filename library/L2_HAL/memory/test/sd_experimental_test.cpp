#include "L2_HAL/memory/sd_experimental.hpp"
#include "L4_Testing/testing_frameworks.hpp"

namespace sjsu::experimental
{
EMIT_ALL_METHODS(Sd);

TEST_CASE("Testing [experimental] SD Card Driver Class", "[sd-experimental]")
{
  Mock<sjsu::Spi> mock_spi;
  Mock<sjsu::Gpio> mock_card_detect;
  Mock<sjsu::Gpio> mock_chip_select;

  Fake(Method(mock_chip_select, SetDirection), Method(mock_chip_select, Set));
  Fake(Method(mock_card_detect, SetDirection));
  Fake(Method(mock_spi, Initialize), Method(mock_spi, SetClock),
       Method(mock_spi, SetDataSize));

  Sd sd(mock_spi.get(), mock_chip_select.get(), mock_card_detect.get());

  SECTION("GetMemoryType()")
  {
    // Verify
    CHECK(sd.GetMemoryType() == Storage::Type::kSD);
  }

  SECTION("Initialize()")
  {
    // Exercise
    sd.Initialize();

    // Verify
    Verify(
        Method(mock_chip_select, SetDirection).Using(Gpio::Direction::kOutput));
    Verify(Method(mock_chip_select, Set).Using(Gpio::State::kHigh));
    Verify(
        Method(mock_card_detect, SetDirection).Using(Gpio::Direction::kInput));
    Verify(Method(mock_spi, Initialize));
    Verify(Method(mock_spi, SetClock)
               .Using(units::frequency::hertz_t{ 400_kHz }, false, false));
    Verify(Method(mock_spi, SetDataSize).Using(Spi::DataSize::kEight));
  }

  SECTION("IsMediaPresent()")
  {
    SECTION("Active Low")
    {
      SECTION("Is Present")
      {
        // Setup: false indicates a low voltage, as the pin signals are all
        // active high by default.
        When(Method(mock_card_detect, Read)).Return(false);

        // Exercise
        bool media_is_present = sd.IsMediaPresent();

        // Verify
        CHECK(media_is_present);
      }

      SECTION("Is NOT Present")
      {
        // Setup: true indicates a high voltage, as the pin signals are all
        // active high by default.
        When(Method(mock_card_detect, Read)).Return(true);

        // Exercise
        bool media_is_present = sd.IsMediaPresent();

        // Verify
        CHECK(!media_is_present);
      }
    }

    SECTION("Active High")
    {
      sjsu::experimental::Sd active_low_sd(
          mock_spi.get(), mock_chip_select.get(), mock_card_detect.get(),
          sd.kDefaultSpiFrequency, sjsu::Gpio::State::kHigh);

      SECTION("Is Present")
      {
        // Setup
        When(Method(mock_card_detect, Read)).Return(true);

        // Exercise
        bool media_is_present = active_low_sd.IsMediaPresent();

        // Verify
        CHECK(media_is_present);
      }

      SECTION("Is NOT Present")
      {
        // Setup
        When(Method(mock_card_detect, Read)).Return(false);

        // Exercise
        bool media_is_present = active_low_sd.IsMediaPresent();

        // Verify
        CHECK(!media_is_present);
      }
    }
  }

  SECTION("Disable()")
  {
    CHECK(Status::kNotImplemented == sd.Disable());
  }

  SECTION("GetBlockSize()")
  {
    CHECK(512_B == sd.GetBlockSize());
  }

  SECTION("IsReadOnly()")
  {
    CHECK(false == sd.IsReadOnly());
  }

  SECTION("GetCapacity()")
  {
    // Setup
    Sd::CardInfo_t & card_info = sd.GetCardInfo();

    constexpr uint32_t kCSize = 0x003B37;

    // Setup: Bits [48:69] are C_SIZE register which contains
    card_info.csd.byte[7] = (kCSize >> 16) & 0xFF;
    card_info.csd.byte[8] = (kCSize >> 8) & 0xFF;
    card_info.csd.byte[9] = (kCSize >> 0) & 0xFF;

    const units::data::byte_t kExpectedSize = (kCSize + 1) * 512_kB;

    // Exercise
    auto actual_size = sd.GetCapacity();

    // Verify
    INFO("Expected = " << kExpectedSize.to<float>()
                       << " :: Actual = " << actual_size.to<float>());
    CHECK(kExpectedSize == actual_size);
  }

  SECTION("Mount()")
  {
    // NOT TESTED!
  }

  SECTION("Read()")
  {
    // NOT TESTED!
  }

  SECTION("Write()")
  {
    // NOT TESTED!
  }
}
}  // namespace sjsu::experimental
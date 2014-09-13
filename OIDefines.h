/* Created and copyrighted by Zachary J. Fields. All rights reserved. */

#ifndef OI_DEFINES_H
#define OI_DEFINES_H

#include <cstdint>

namespace roomba {
namespace series500 {
/// \brief Baud (OpCode 129)
enum BaudCode : uint8_t {
	BAUD_300 = 0,
	BAUD_600,
	BAUD_1200,
	BAUD_2400,
	BAUD_4800,
	BAUD_9600,
	BAUD_14400,
	BAUD_19200,
	BAUD_28800,
	BAUD_38400,
	BAUD_57600,
	BAUD_115200,
};

/// \brief Song (OpCode 140)
enum Pitch : uint8_t {
	REST = 30,
	G1 = 31,
	G1_SHARP = 32,
	A1 = 33,
	A1_SHARP = 34,
	B1 = 35,
	C2 = 36,
	C2_SHARP = 37,
	D2 = 38,
	D2_SHARP = 39,
	E2 = 40,
	F2 = 41,
	F2_SHARP = 42,
	G2 = 43,
	G2_SHARP = 44,
	A2 = 45,
	A2_SHARP = 46,
	B2 = 47,
	C3 = 48,
	C3_SHARP = 49,
	D3 = 50,
	D3_SHARP = 51,
	E3 = 52,
	F3 = 53,
	F3_SHARP = 54,
	G3 = 55,
	G3_SHARP = 56,
	A3 = 57,
	A3_SHARP = 58,
	B3 = 59,
	C4 = 60,
	MIDDLE_C = 60,
	C4_SHARP = 61,
	D4 = 62,
	D4_SHARP = 63,
	E4 = 64,
	F4 = 65,
	F4_SHARP = 66,
	G4 = 67,
	G4_SHARP = 68,
	A4 = 69,
	A4_SHARP = 70,
	B4 = 71,
	C5 = 72,
	C5_SHARP = 73,
	D5 = 74,
	D5_SHARP = 75,
	E5 = 76,
	F5 = 77,
	F5_SHARP = 78,
	G5 = 79,
	G5_SHARP = 80,
	A5 = 81,
	A5_SHARP = 82,
	B5 = 83,
	C6 = 84,
	C6_SHARP = 85,
	D6 = 86,
	D6_SHARP = 87,
	E6 = 88,
	F6 = 89,
	F6_SHARP = 90,
	G6 = 91,
	G6_SHARP = 92,
	A6 = 93,
	A6_SHARP = 94,
	B6 = 95,
	C7 = 96,
	C7_SHARP = 97,
	D7 = 98,
	D7_SHARP = 99,
	E7 = 100,
	F7 = 101,
	F7_SHARP = 102,
	G7 = 103,
	G7_SHARP = 104,
	A7 = 105,
	A7_SHARP = 106,
	B7 = 107,
};

/// \brief Set Day/Time (OpCode 168)
enum Day : uint8_t {
	SUNDAY = 0,
	MONDAY,
	TUESDAY,
	WEDNESDAY,
	THURSDAY,
	FRIDAY,
	SATURDAY,
};

/// \brief Charging State (PacketId 21)
enum ChargingState : uint8_t {
	NOT_CHARGING = 0,
	RECONDITIONING_CHARGING,
	FULL_CHARGING,
	TRICKLE_CHARGING,
	WAITING,
	CHARGING_FAULT,
};

/// \brief OI Mode (PacketId 35)
enum OIMode : uint8_t {
	OFF = 0,
	PASSIVE,
	SAFE,
	FULL,
};

namespace bitmask {
/// \brief Scheduling LEDs (OpCode 162) and Schedule (OpCode 167)
enum Days : uint8_t {
	DISABLE = 0x00,
	SUNDAY = 0x01,
	MONDAY = 0x02,
	TUESDAY = 0x04,
	WEDNESDAY = 0x08,
	THURSDAY = 0x10,
	FRIDAY = 0x20,
	SATURDAY = 0x40,
};

/// \brief Buttons (OpCode 165 / PacketId 18)
enum Buttons : uint8_t {
	CLEAN = 0x01,
	SPOT = 0x02,
	DOCK = 0x04,
	MINUTE = 0x08,
	HOUR = 0x10,
	DAY = 0x20,
	SCHEDULE = 0x40,
	CLOCK = 0x80,
};

/// \brief Motor Overcurrents (PacketId 14)
enum Motors : uint8_t {
	SIDE_BRUSH = 0x01,
	MAIN_BRUSH = 0x04,
	RIGHT_WHEEL = 0x08,
	LEFT_WHEEL = 0x10,
};

/// \brief Motor States (OpCode 138)
enum MotorStates : uint8_t {
	SIDE_BRUSH_ENGAGED = 0x01,
	VACUUM_ENGAGED = 0x02,
	MAIN_BRUSH_ENGAGED = 0x04,
	SIDE_BRUSH_CLOCKWISE = 0x08,
	MAIN_BRUSH_OUTWARD = 0x10,
};

namespace display {
/// \brief LEDs (OpCode 139)
enum LEDs : uint8_t {
	DEBRIS = 0x01,
	SPOT = 0x02,
	DOCK = 0x04,
	CHECK_ROBOT = 0x08,
};

/// \brief Scheduling LEDs (OpCode 162)
enum SchedulingLEDs : uint8_t {
	COLON = 0x01,
	PM = 0x02,
	AM = 0x04,
	CLOCK = 0x08,
	SCHEDULE = 0x10,
};

/// \brief Digit LEDs Raw (OpCode 163)
/// \note FAAAB
/// \note F   B
/// \note  GGG
/// \note E   C
/// \note EDDDC
enum DigitN : uint8_t {
	A = 0x01,
	B = 0x02,
	C = 0x04,
	D = 0x08,
	E = 0x10,
	F = 0x20,
	G = 0x40,
};
} // namespace display

/// \brief Bumps and Wheel Drops (PacketId 7)
enum BumpsAndWheelDrops : uint8_t {
	BUMP_RIGHT = 0x01,
	BUMP_LEFT = 0x02,
	WHEEL_DROP_RIGHT = 0x04,
	WHEEL_DROP_LEFT = 0x08,
};

/// \brief Charging Sources Available (PacketId 34)
enum ChargersAvailable : uint8_t {
	INTERNAL_CHARGER = 0x01,
	HOME_BASE = 0x02,
};

/// \brief Light Bumper (PacketId 45)
enum LightBumpers : uint8_t {
	LEFT = 0x01,
	FRONT_LEFT = 0x02,
	CENTER_LEFT = 0x04,
	CENTER_RIGHT = 0x08,
	FRONT_RIGHT = 0x10,
	RIGHT = 0x20,
};
} // namespace bitmask

namespace command {
/// \brief Operation codes for the Roomba Open Interface (OI) Specification
enum OpCode : uint8_t {
	START = 128,
	PASSIVE = 128,
	BAUD = 129,
	CONTROL = 130,
	SAFE = 131,
	FULL = 132,
	POWER = 133,
	SPOT = 134,
	CLEAN = 135,
	MAX = 136,
	DRIVE = 137,
	MOTORS = 138,
	LEDS = 139,
	SONG = 140,
	PLAY = 141,
	SENSORS = 142,
	SEEK_DOCK = 143,
	PWM_MOTORS = 144,
	DRIVE_DIRECT = 145,
	DRIVE_PWM = 146,
	STREAM = 148,
	QUERY_LIST = 149,
	PAUSE_RESUME_STREAM = 150,
	SCHEDULING_LEDS = 162,
	DIGIT_LEDS_RAW = 163,
	DIGIT_LEDS_ASCII = 164,
	BUTTONS = 165,
	SCHEDULE = 167,
	SET_DAY_TIME = 168,
};
} // namespace command

namespace sensor {
/// \brief Roomba Open Interface Sensor Packet Identification numbers
enum PacketId : uint8_t {
	PACKETS_7_THRU_26 = 0,
	PACKETS_7_THRU_16 = 1,
	PACKETS_17_THRU_20 = 2,
	PACKETS_21_THRU_26 = 3,
	PACKETS_27_THRU_34 = 4,
	PACKETS_35_THRU_42 = 5,
	PACKETS_7_THRU_42 = 6,
	BUMPS_AND_WHEEL_DROPS = 7,
	WALL = 8,
	CLIFF_LEFT = 9,
	CLIFF_FRONT_LEFT = 10,
	CLIFF_FRONT_RIGHT = 11,
	CLIFF_RIGHT = 12,
	VIRTUAL_WALL = 13,
	MOTOR_OVERCURRENTS = 14,
	DIRT_DETECT = 15,
	INFRARED_CHARACTER_OMNI = 17,
	BUTTONS = 18,
	DISTANCE = 19,
	ANGLE = 20,
	CHARGING_STATE = 21,
	VOLTAGE = 22,
	CURRENT = 23,
	TEMPERATURE = 24,
	BATTERY_CHARGE = 25,
	BATTERY_CAPACITY = 26,
	WALL_SIGNAL = 27,
	CLIFF_LEFT_SIGNAL = 28,
	CLIFF_FRONT_LEFT_SIGNAL = 29,
	CLIFF_FRONT_RIGHT_SIGNAL = 30,
	CLIFF_RIGHT_SIGNAL = 31,
	CHARGING_SOURCES_AVAILABLE = 34,
	OI_MODE = 35,
	SONG_NUMBER = 36,
	SONG_PLAYING = 37,
	NUMBER_OF_STREAM_PACKETS = 38,
	REQUESTED_VELOCITY = 39,
	REQUESTED_RADIUS = 40,
	REQUESTED_RIGHT_VELOCITY = 41,
	REQUESTED_LEFT_VELOCITY = 42,
	RIGHT_ENCODER_COUNTS = 43,
	LEFT_ENCODER_COUNTS = 44,
	LIGHT_BUMPER = 45,
	LIGHT_BUMP_LEFT_SIGNAL = 46,
	LIGHT_BUMP_FRONT_LEFT_SIGNAL = 47,
	LIGHT_BUMP_CENTER_LEFT_SIGNAL = 48,
	LIGHT_BUMP_CENTER_RIGHT_SIGNAL = 49,
	LIGHT_BUMP_FRONT_RIGHT_SIGNAL = 50,
	LIGHT_BUMP_RIGHT_SIGNAL = 51,
	INFRARED_CHARACTER_LEFT = 52,
	INFRARED_CHARACTER_RIGHT = 53,
	LEFT_MOTOR_CURRENT = 54,
	RIGHT_MOTOR_CURRENT = 55,
	MAIN_BRUSH_MOTOR_CURRENT = 56,
	SIDE_BRUSH_MOTOR_CURRENT = 57,
	STASIS = 58,
	ALL_SENSOR_DATA = 100,
	PACKETS_7_THRU_58 = 100,
	PACKETS_43_THRU_58 = 101,
	PACKETS_46_THRU_51 = 106,
	PACKETS_54_THRU_58 = 107,
};
} // namespace sensor
} // namespace series500
} // namespace roomba

#endif

/* Created and copyrighted by Zachary J. Fields. All rights reserved. */

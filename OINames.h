/* Created and copyrighted by Zachary J. Fields. All rights reserved. */

#ifndef OI_NAMES_H
#define OI_NAMES_H

#include <cstdint>

namespace roomba {
namespace series500 {
// Baud (OpCode 129)
enum BaudRate : uint8_t {
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

// Song (OpCode 140)
enum Note : uint8_t {
	G1 = 31,
	G1_SHARP,
	A1,
	A1_SHARP,
	B1,
	C2,
	C2_SHARP,
	D2,
	D2_SHARP,
	E2,
	F2,
	F2_SHARP,
	G2,
	G2_SHARP,
	A2,
	A2_SHARP,
	B2,
	C3,
	C3_SHARP,
	D3,
	D3_SHARP,
	E3,
	F3,
	F3_SHARP,
	G3,
	G3_SHARP,
	A3,
	A3_SHARP,
	B3,
	C4,
	C4_SHARP,
	D4,
	D4_SHARP,
	E4,
	F4,
	F4_SHARP,
	G4,
	G4_SHARP,
	A4,
	A4_SHARP,
	B4,
	C5,
	C5_SHARP,
	D5,
	D5_SHARP,
	E5,
	F5,
	F5_SHARP,
	G5,
	G5_SHARP,
	A5,
	A5_SHARP,
	B5,
	C6,
	C6_SHARP,
	D6,
	D6_SHARP,
	E6,
	F6,
	F6_SHARP,
	G6,
	G6_SHARP,
	A6,
	A6_SHARP,
	B6,
	C7,
	C7_SHARP,
	D7,
	D7_SHARP,
	E7,
	F7,
	F7_SHARP,
	G7,
	G7_SHARP,
	A7,
	A7_SHARP,
	B7,
};

// Set Day/Time (OpCode 168)
enum Day : uint8_t {
	SUNDAY = 0,
	MONDAY,
	TUESDAY,
	WEDNESDAY,
	THURSDAY,
	FRIDAY,
	SATURDAY,
};

// Charging State (Packet 21)
enum ChargingState : uint8_t {
	NOT_CHARGING = 0,
	RECONDITIONING_CHARGING,
	FULL_CHARGING,
	TRICKLE_CHARGING,
	WAITING,
	CHARGING_FAULT,
};

// OI Mode (Packet 35)
enum OIMode : uint8_t {
	OFF = 0,
	PASSIVE,
	SAFE,
	FULL,
};

namespace bitmasks {
// Scheduling LEDs (OpCode 162) and Schedule (OpCode 167)
enum WeekDay : uint8_t {
	SUNDAY = 0x01,
	MONDAY = 0x02,
	TUESDAY = 0x04,
	WEDNESDAY = 0x08,
	THURSDAY = 0x10,
	FRIDAY = 0x20,
	SATURDAY = 0x40,
};

// Buttons (OpCode 165 / Packet 18)
enum Button : uint8_t {
	CLEAN = 0x01,
	SPOT = 0x02,
	DOCK = 0x04,
	MINUTE = 0x08,
	HOUR = 0x10,
	DAY = 0x20,
	SCHEDULE = 0x40,
	CLOCK = 0x80,
};

// Motor Overcurrents (Packet 14)
enum Motor : uint8_t {
	SIDE_BRUSH = 0x01,
	MAIN_BRUSH = 0x04,
	RIGHT_WHEEL = 0x08,
	LEFT_WHEEL = 0x10,
};

// Motors (OpCode 138)
enum MotorState : uint8_t {
	SIDE_BRUSH_ENGAGED = 0x01,
	VACUUM_ENGAGED = 0x02,
	MAIN_BRUSH_ENGAGED = 0x04,
	SIDE_BRUSH_CLOCKWISE = 0x08,
	MAIN_BRUSH_OUTWARD = 0x10,
};

namespace display {
// LEDs (OpCode 139)
enum LED : uint8_t {
	DEBRIS = 0x01,
	SPOT = 0x02,
	DOCK = 0x04,
	CHECK_ROBOT = 0x08,
};

// Scheduling LEDs (OpCode 162)
enum SchedulingLEDs : uint8_t {
	COLON = 0x01,
	PM = 0x02,
	AM = 0x04,
	CLOCK = 0x08,
	SCHEDULE = 0x10,
};

// Digit LEDs Raw (OpCode 163)
//
//  FAAAB
//  F   B
//   GGG
//  E   C
//  EDDDC
//
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

// Bumps and Wheel Drops (Packet 7)
enum BumpsAndWheelDrops : uint8_t {
	BUMP_RIGHT = 0x01,
	BUMP_LEFT = 0x02,
	WHEEL_DROP_RIGHT = 0x04,
	WHEEL_DROP_LEFT = 0x08,
};

// Charging Sources Available (Packet 34)
enum ChargerAvailable : uint8_t {
	INTERNAL_CHARGER = 0x01,
	HOME_BASE = 0x02,
};

// Light Bumper (Packet 45)
enum LightBumper : uint8_t {
	LEFT = 0x01,
	FRONT_LEFT = 0x02,
	CENTER_LEFT = 0x04,
	CENTER_RIGHT = 0x08,
	FRONT_RIGHT = 0x10,
	RIGHT = 0x20,
};
} // namespace bitmask

namespace commands {
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
} // namespace commands

namespace sensors {
enum Packet : uint8_t {
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
};
} // namespace sensors
} // namespace series500
} // namespace roomba

#endif

/* Created and copyrighted by Zachary J. Fields. All rights reserved. */

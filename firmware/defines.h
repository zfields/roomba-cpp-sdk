/* Created and copyrighted by Zachary J. Fields. Offered as open source under the MIT License (MIT). */

#ifndef DEFINES_H
#define DEFINES_H

#ifndef DISABLE_SENSORS
#define SENSORS_ENABLED
#endif

#define ROOMBA_CPP_SDK
#define ROOMBA_CPP_SDK_VERSION 1.0.0-alpha

// Setting integer optimization for speed
#define uint_opt8_t uint_fast8_t
#define uint_opt16_t uint_fast16_t
#define uint_opt32_t uint_fast32_t
#define uint_opt64_t uint_fast64_t
#define int_opt8_t int_fast8_t
#define int_opt16_t int_fast16_t
#define int_opt32_t int_fast32_t
#define int_opt64_t int_fast64_t

#include <cstdint>

/// \brief The iRobot Roomba autonomous robotic vacuum cleaner
/// \details Roomba was introduced in 2002. As of Feb 2014, over
/// 10 million units have been sold worldwide. Roomba features a
/// set of basic sensors that help it perform tasks. For instance,
/// the Roomba is able to change direction on encountering obstacles,
/// detect dirty spots on the floor, and detect steep drops to keep
/// it from falling down stairs. It uses two independently operating
/// wheels that allow 360 degree turns. Additionally, it can adapt
/// to perform other more "creative" tasks using an embedded computer
/// in conjunction with the Roomba Open Interface.
namespace roomba {

/// \brief Open Interface Series
enum OISeries {
	SCI,
	OI500,
	OI600,
};

/// \brief Return codes
enum ReturnCode : int_opt8_t {
	INVALID_CHECKSUM = -101,
	SERIAL_TRANSFER_FAILURE = -100,
	INVALID_PARAMETER = -10,
	NO_DATA_AVAILABLE = -4,
	FAILURE_TO_SYNC = -3,
	INVALID_MODE_FOR_REQUESTED_OPERATION = -2,
	OI_NOT_STARTED = -1,
	SUCCESS = 0,
};

/// \brief Baud (OpCode 129)
enum BaudCode : uint_opt8_t {
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
enum Pitch : uint_opt8_t {
	REST = 30,
	G_1 = 31,
	G_1_SHARP = 32,
	A_1 = 33,
	A_SHARP_1 = 34,
	B_1 = 35,
	C_2 = 36,
	C_SHARP_2 = 37,
	D_2 = 38,
	D_SHARP_2 = 39,
	E_2 = 40,
	F_2 = 41,
	F_SHARP_2 = 42,
	G_2 = 43,
	G_SHARP_2 = 44,
	A_2 = 45,
	A_SHARP_2 = 46,
	B_2 = 47,
	C_3 = 48,
	C_SHARP_3 = 49,
	D_3 = 50,
	D_SHARP_3 = 51,
	E_3 = 52,
	F_3 = 53,
	F_SHARP_3 = 54,
	G_3 = 55,
	G_SHARP_3 = 56,
	A_3 = 57,
	A_SHARP_3 = 58,
	B_3 = 59,
	C_4 = 60,
	MIDDLE_C = 60,
	C_SHARP_4 = 61,
	D_4 = 62,
	D_SHARP_4 = 63,
	E_4 = 64,
	F_4 = 65,
	F_SHARP_4 = 66,
	G_4 = 67,
	G_SHARP_4 = 68,
	A_4 = 69,
	A_SHARP_4 = 70,
	B_4 = 71,
	C_5 = 72,
	C_SHARP_5 = 73,
	D_5 = 74,
	D_SHARP_5 = 75,
	E_5 = 76,
	F_5 = 77,
	F_SHARP_5 = 78,
	G_5 = 79,
	G_SHARP_5 = 80,
	A_5 = 81,
	A_SHARP_5 = 82,
	B_5 = 83,
	C_6 = 84,
	C_SHARP_6 = 85,
	D_6 = 86,
	D_SHARP_6 = 87,
	E_6 = 88,
	F_6 = 89,
	F_SHARP_6 = 90,
	G_6 = 91,
	G_SHARP_6 = 92,
	A_6 = 93,
	A_SHARP_6 = 94,
	B_6 = 95,
	C_7 = 96,
	C_SHARP_7 = 97,
	D_7 = 98,
	D_SHARP_7 = 99,
	E_7 = 100,
	F_7 = 101,
	F_SHARP_7 = 102,
	G_7 = 103,
	G_SHARP_7 = 104,
	A_7 = 105,
	A_SHARP_7 = 106,
	B_7 = 107,
};

/// \brief Set Day/Time (OpCode 168)
enum Day : uint_opt8_t {
	SUNDAY = 0,
	MONDAY,
	TUESDAY,
	WEDNESDAY,
	THURSDAY,
	FRIDAY,
	SATURDAY,
};

/// \brief Charging State (PacketId 21)
enum ChargingState : uint_opt8_t {
	NOT_CHARGING = 0,
	RECONDITIONING_CHARGING,
	FULL_CHARGING,
	TRICKLE_CHARGING,
	WAITING,
	CHARGING_FAULT,
};

/// \brief OI Mode (PacketId 35)
enum OIMode : uint_opt8_t {
	OFF = 0,
	PASSIVE,
	SAFE,
	FULL,
};

namespace bitmask {
/// \brief Scheduling LEDs (OpCode 162) and Schedule (OpCode 167)
enum Days : uint_opt8_t {
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
enum Buttons : uint_opt8_t {
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
enum Motors : uint_opt8_t {
	SIDE_BRUSH = 0x01,
	MAIN_BRUSH = 0x04,
	RIGHT_WHEEL = 0x08,
	LEFT_WHEEL = 0x10,
};

/// \brief Motor States (OpCode 138)
enum MotorStates : uint_opt8_t {
	SIDE_BRUSH_ENGAGED = 0x01,
	VACUUM_ENGAGED = 0x02,
	MAIN_BRUSH_ENGAGED = 0x04,
	SIDE_BRUSH_CLOCKWISE = 0x08,
	MAIN_BRUSH_OUTWARD = 0x10,
};

namespace display {
/// \brief LEDs (OpCode 139)
enum LEDs : uint_opt8_t {
	DEBRIS = 0x01,
	SPOT = 0x02,
	DOCK = 0x04,
	CHECK_ROBOT = 0x08,
};

/// \brief Scheduling LEDs (OpCode 162)
enum SchedulingLEDs : uint_opt8_t {
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
enum DigitN : uint_opt8_t {
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
enum BumpsAndWheelDrops : uint_opt8_t {
	BUMP_RIGHT = 0x01,
	BUMP_LEFT = 0x02,
	WHEEL_DROP_RIGHT = 0x04,
	WHEEL_DROP_LEFT = 0x08,
};

/// \brief Charging Sources Available (PacketId 34)
enum ChargersAvailable : uint_opt8_t {
	INTERNAL_CHARGER = 0x01,
	HOME_BASE = 0x02,
};

/// \brief Light Bumper (PacketId 45)
enum LightBumpers : uint_opt8_t {
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
enum OpCode : uint_opt8_t {
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
/// \brief iRobot® Roomba Open Interface (OI) Sensor Packets
enum PacketId : uint_opt8_t {
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
} // sensor
} // namespace roomba

#endif

/* Created and copyrighted by Zachary J. Fields. Offered as open source under the MIT License (MIT). */

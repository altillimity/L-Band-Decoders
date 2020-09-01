#include <iostream>
#include <fstream>
#include <complex>
#include <vector>
#include <ctime>

// Processing buffer size
#define BUFFER_SIZE (88)

int main(int argc, char *argv[])
{
    // Input file
    std::ifstream data_in(argv[1], std::ios::binary);

    // Read buffer
    uint8_t buffer[BUFFER_SIZE];

    // Frame counter
    int frame = 0;

    // Read until EOF
    while (!data_in.eof())
    {
        // Read buffer
        data_in.read((char *)buffer, BUFFER_SIZE);

        int frame_type = buffer[4] > 100 ? 1 : 0;
        std::cout << "Frame type : " << frame_type << std::endl;

        if (frame_type == 0)
        {
            // Onboard time
            unsigned long clock_time = buffer[6] << 24 | buffer[7] << 16 | buffer[8] << 8 | buffer[9];
            std::tm *clock_time_human = gmtime((const time_t *)&clock_time);

            // Onboard clock mode
            uint32_t clock_mode = buffer[10] << 24 | buffer[11] << 16 | buffer[12] << 8 | buffer[13];

            // Satellite coordinates
            uint64_t coordinate_x = buffer[14] << 56 | buffer[15] << 48 | buffer[16] << 40 | buffer[17] << 32 | buffer[18] << 24 | buffer[19] << 16 | buffer[20] << 8 | buffer[21];
            uint64_t coordinate_y = buffer[22] << 56 | buffer[23] << 48 | buffer[24] << 40 | buffer[25] << 32 | buffer[26] << 24 | buffer[27] << 16 | buffer[28] << 8 | buffer[29];
            uint64_t coordinate_z = buffer[30] << 56 | buffer[31] << 48 | buffer[32] << 40 | buffer[33] << 32 | buffer[34] << 24 | buffer[35] << 16 | buffer[36] << 8 | buffer[37];

            // Satellite velocity
            uint8_t velocity_vx = buffer[38];
            uint8_t velocity_vy = buffer[42];
            uint8_t velocity_vz = buffer[46];

            // Time of last acquired anguler velocity data
            uint64_t last_angular_velocity_time = buffer[56] << 40 | buffer[57] << 32 | buffer[58] << 24 | buffer[59] << 16 | buffer[60] << 8 | buffer[61];

            // Angular velocities
            uint32_t angular_velocity_wx1 = buffer[62] << 24 | buffer[63] << 16 | buffer[64] << 8 | buffer[65];
            uint32_t angular_velocity_wy1 = buffer[66] << 24 | buffer[67] << 16 | buffer[68] << 8 | buffer[69];
            uint32_t angular_velocity_wy2 = buffer[70] << 24 | buffer[71] << 16 | buffer[72] << 8 | buffer[73];
            uint32_t angular_velocity_wz2 = buffer[74] << 24 | buffer[75] << 16 | buffer[76] << 8 | buffer[77];
            uint32_t angular_velocity_wz3 = buffer[78] << 24 | buffer[79] << 16 | buffer[80] << 8 | buffer[81];
            uint32_t angular_velocity_wx3 = buffer[82] << 24 | buffer[83] << 16 | buffer[84] << 8 | buffer[85];

            // Angular velocity sensor status
            uint8_t angular_velocity_sensor_status = buffer[86];

            // Print it out
            std::cout << "   Satellite time (UTC)           : " << std::to_string(clock_time_human->tm_hour) + ":" + (clock_time_human->tm_min > 9 ? std::to_string(clock_time_human->tm_min) : "0" + std::to_string(clock_time_human->tm_min)) + ":" + (clock_time_human->tm_sec > 9 ? std::to_string(clock_time_human->tm_sec) : "0" + std::to_string(clock_time_human->tm_sec)) << std::endl;
            std::cout << "   Satellite clock mode           : " << clock_mode << std::endl;
            std::cout << "   Coordinates (X)                : " << coordinate_x << std::endl;
            std::cout << "   Coordinates (Y)                : " << coordinate_y << std::endl;
            std::cout << "   Coordinates (Z)                : " << coordinate_z << std::endl;
            std::cout << "   Velocity (X)                   : " << (int)velocity_vx << std::endl;
            std::cout << "   Velocity (Y)                   : " << (int)velocity_vy << std::endl;
            std::cout << "   Velocity (Z)                   : " << (int)velocity_vz << std::endl;
            std::cout << "   Last angular velocity time     : " << last_angular_velocity_time << std::endl;
            std::cout << "   Angular velocity Sensor 1 (X1) : " << angular_velocity_wx1 << std::endl;
            std::cout << "   Angular velocity Sensor 1 (Y1) : " << angular_velocity_wy1 << std::endl;
            std::cout << "   Angular velocity Sensor 1 (Y2) : " << angular_velocity_wy2 << std::endl;
            std::cout << "   Angular velocity Sensor 1 (Z2) : " << angular_velocity_wz2 << std::endl;
            std::cout << "   Angular velocity Sensor 1 (Z3) : " << angular_velocity_wz3 << std::endl;
            std::cout << "   Angular velocity Sensor 1 (X3) : " << angular_velocity_wx3 << std::endl;
            std::cout << "   Angular velocity sensor status : " << last_angular_velocity_time << std::endl;
        }
        else
        {
            // Onboard clock time scale
            uint32_t clock_scale = buffer[6] << 24 | buffer[7] << 16 | buffer[8] << 8 | buffer[9];

            // SAT Status word
            uint16_t sat_status = buffer[10] << 8 | buffer[11];

            // BOKS-M Start tracker measurement time
            uint32_t star_tracker_measurement_time = buffer[12] << 24 | buffer[13] << 16 | buffer[14] << 8 | buffer[15];

            // Location of body-fixed coordinate system relative to default coordinate system
            uint32_t location_relative_default = buffer[16] << 24 | buffer[17] << 16 | buffer[18] << 8 | buffer[19];

            // Time of clock word generation
            uint32_t clock_word_gen_time = buffer[32] << 24 | buffer[33] << 16 | buffer[34] << 8 | buffer[35];

            // Satellite clock status
            uint16_t clock_status = buffer[36] << 8 | buffer[37];

            // Last pitch roll data update
            uint64_t last_pitch_roll_update = buffer[38] << 40 | buffer[39] << 32 | buffer[40] << 24 | buffer[41] << 16 | buffer[42] << 8 | buffer[43];

            // Sensor 1 & 2 angle data
            uint16_t pitch_angle_1 = buffer[44] << 8 | buffer[45];
            uint16_t roll_angle_1 = buffer[46] << 8 | buffer[47];
            uint16_t pitch_angle_2 = buffer[48] << 8 | buffer[49];
            uint16_t roll_angle_2 = buffer[50] << 8 | buffer[51];

            // Timestamp delay relative to clock
            uint32_t clock_timestamp_delay = buffer[52] << 24 | buffer[53] << 16 | buffer[54] << 8 | buffer[55];

            // Last angular velocity measurement time
            uint64_t last_angular_velocity_time = buffer[56] << 40 | buffer[57] << 32 | buffer[58] << 24 | buffer[59] << 16 | buffer[60] << 8 | buffer[61];

            // Angular velocities
            uint32_t angular_velocity_wx1 = buffer[62] << 24 | buffer[63] << 16 | buffer[64] << 8 | buffer[65];
            uint32_t angular_velocity_wy1 = buffer[66] << 24 | buffer[67] << 16 | buffer[68] << 8 | buffer[69];
            uint32_t angular_velocity_wy2 = buffer[70] << 24 | buffer[71] << 16 | buffer[72] << 8 | buffer[73];
            uint32_t angular_velocity_wz2 = buffer[74] << 24 | buffer[75] << 16 | buffer[76] << 8 | buffer[77];
            uint32_t angular_velocity_wz3 = buffer[78] << 24 | buffer[79] << 16 | buffer[80] << 8 | buffer[81];
            uint32_t angular_velocity_wx3 = buffer[82] << 24 | buffer[83] << 16 | buffer[84] << 8 | buffer[85];

             // Angular velocity sensor status
            uint8_t angular_velocity_sensor_status = buffer[86];

            // Print it out
            std::cout << "   Satellite clock scale                             : " << clock_scale << std::endl;
            std::cout << "   SAT Status                                        : " << sat_status << std::endl;
            std::cout << "   BOKS-M Measurement time                           : " << star_tracker_measurement_time << std::endl;
            std::cout << "   Location of relative/default coordinate system    : " << location_relative_default << std::endl;
            std::cout << "   Clock status word generation time                 : " << clock_word_gen_time << std::endl;
            std::cout << "   Clock status                                      : " << clock_status << std::endl;
            std::cout << "   Last pitch/roll update                            : " << last_pitch_roll_update << std::endl;
            std::cout << "   Pitch Sensor 1 (°)                                : " << pitch_angle_1 << std::endl;
            std::cout << "   Roll Sensor 1 (°)                                 : " << roll_angle_1 << std::endl;
            std::cout << "   Pitch Sensor 2 (°)                                : " << pitch_angle_2 << std::endl;
            std::cout << "   Roll Sensor 2 (°)                                 : " << roll_angle_2 << std::endl;
            std::cout << "   Timestamp delay relative to clock                 : " << clock_timestamp_delay << std::endl;
            std::cout << "   Last angular velocity time                        : " << last_angular_velocity_time << std::endl;
            std::cout << "   Angular velocity Sensor 1 (X1)                    : " << angular_velocity_wx1 << std::endl;
            std::cout << "   Angular velocity Sensor 1 (Y1)                    : " << angular_velocity_wy1 << std::endl;
            std::cout << "   Angular velocity Sensor 2 (Y2)                    : " << angular_velocity_wy2 << std::endl;
            std::cout << "   Angular velocity Sensor 2 (Z2)                    : " << angular_velocity_wz2 << std::endl;
            std::cout << "   Angular velocity Sensor 3 (Z3)                    : " << angular_velocity_wz3 << std::endl;
            std::cout << "   Angular velocity Sensor 3 (X3)                    : " << angular_velocity_wx3 << std::endl;
            std::cout << "   Angular velocity sensor status                    : " << last_angular_velocity_time << std::endl;
        }

        // Frame counter
        frame++;
    }

    data_in.close();
}
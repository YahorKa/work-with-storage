//#include <GCS_MAVLink/GCS.h>
#include <AP_HAL/AP_HAL.h>
#include <StorageManager/StorageManager.h>
#include <GCS_MAVLink/GCS.h>
#include <AP_Common/Bitmask.h>

class AP_UVRLoginAccess
{
    //login password
    struct loginPassword{
        char login[30];
        char password[200];
    };

    enum command_type{
        ADD_PILOT = 0,
        CHECK_PILOT,
        DELETE_PILOT
    };
    //function compare structure
    inline bool str_compare(loginPassword * data ,const loginPassword& pilot){
        if( strcmp(data->login,pilot.login)  == 0){
            if( strcmp(data->password,pilot.password)  == 0){
                return true;
            }
            gcs().send_text(MAV_SEVERITY_WARNING, "ERROR: password is not correct");
        }
        gcs().send_text(MAV_SEVERITY_WARNING, "ERROR: No pilot with name %s",pilot.login);
        return false;
     }
    //Command from mavlink 
    uint8_t  command;
    //mask for saving pilots
    uint8_t  mask_pilot;
    //private constructor for get instance
    AP_UVRLoginAccess();
    static AP_UVRLoginAccess *_instance; 
    // storage object
    StorageAccess _storage;
    //mask of existing pilot in eeprom
    Bitmask<8> pilotMask;


    public:
    /* Do not allow copies */
    AP_UVRLoginAccess(const AP_UVRLoginAccess &other) = delete;
    AP_UVRLoginAccess &operator=(const AP_UVRLoginAccess&) = delete; 
    static AP_UVRLoginAccess* get_instance();
    //initialize
    void setup();

    //handle MAVLink messages: login and password of pilot and command field (check pilot or add pilot)
    void handle_pilot_message(const mavlink_message_t &msg);

    //compare pilot login and password with hardware storage of pilots.
    bool check_pilot_password(const struct loginPassword &_newPilot);

    //add new pilot to the storage
    bool add_pilot(const struct loginPassword &_newPilot);

    //getter
    void get_pilot_mask();
};
#include "storage.h"

#define DO_INITIALISATION       1
#define LOGIN_DATA_MAGIC_NUMBER 0
#define LOGIN_DATA_MAGIC_LEN    ?
#define MAX_PILOT_NUMBER        8
#define STORAGE_OFFSET          128

extern const AP_HAL::HAL& hal;
//CONSTRUKTOR
AP_UVRLoginAccess::AP_UVRLoginAccess():_storage(StorageAccess(StorageManager::StorageRally)),
                                        pilotMask()
{
 pilotMask.set(3);                                             //for debug
}
AP_UVRLoginAccess* AP_UVRLoginAccess::_instance;
AP_UVRLoginAccess* AP_UVRLoginAccess::get_instance(){
    if(_instance==nullptr) {
        _instance = new AP_UVRLoginAccess();
    }
    return _instance;
}
// test working manager read storage
void AP_UVRLoginAccess::setup(void)
{
#if DO_INITIALISATION
        //write block to storage
        // uint16_t _st;
        // loginPassword str;
        // strncpy(str.login,"Yahor",sizeof(str.login));
        // strncpy(str.password,"qwe123",sizeof(str.password));
        // add_pilot(str);
        // _st = _storage.size();
        // gcs().send_text(MAV_SEVERITY_WARNING, "storage %u",_st);
        //->write block to the storage
        
         get_pilot_mask();
        //read pilot form storage
        loginPassword str;
        strncpy(str.login,"Yahor1",sizeof(str.login));
        strncpy(str.password,"qwe123",sizeof(str.password));
        check_pilot_password(str);
        //->read pilot from storage


#endif        
}

bool AP_UVRLoginAccess::check_pilot_password(const struct loginPassword &_newPilot){
    loginPassword *data;
    loginPassword _check_pilot;
    data = &_check_pilot;
    bool res;
    //for (int i=0; i<pilotMask; i++){
        //if (pilotMask&i){
            res = _storage.read_block(data,LOGIN_DATA_MAGIC_NUMBER,sizeof(struct loginPassword));
                if(str_compare(data,_newPilot)){
                    gcs().send_text(MAV_SEVERITY_WARNING, "ERROR: PILOT EXISTING %s",_newPilot.login);                         //FOR DEBUG
                    return true;}
                if (!res){
                    gcs().send_text(MAV_SEVERITY_WARNING, "ERROR: cant read storage");
                    return false;
                }
        //}
    //}
    return false;
}
//write new pilot login password to the storage
bool AP_UVRLoginAccess::add_pilot(const loginPassword &_newPilot){
    //add pilot has access only without arm add supported by GCS
    //TO DO check mask and have adress from mask
    if (_storage.size() < sizeof(_newPilot)) {
        return false;
    }

   if (!_storage.write_block(LOGIN_DATA_MAGIC_NUMBER ,&_newPilot, sizeof(struct loginPassword))){
        gcs().send_text(MAV_SEVERITY_WARNING, "ERROR: cant add pilot");
        return false;
   }
   gcs().send_text(MAV_SEVERITY_WARNING, "NEW PILOT ADDED");
   return true;
}

void AP_UVRLoginAccess::handle_pilot_message(const mavlink_message_t &msg){

    // armed means the pilot is accessed
    if (hal.util->get_soft_armed()) {
        gcs().send_text(MAV_SEVERITY_WARNING, "ERROR: pilot is already armed");
        return;
    }   
    // now we have to get some type of mav message (waiting from EA), now for Example it will be "mavlink_setup_signing_t"
    mavlink_play_tune_t packet;
    mavlink_msg_play_tune_decode (&msg, &packet);
    loginPassword _loginPassword;
    strncpy(_loginPassword.login,packet.tune,sizeof(packet.tune));
    strncpy(_loginPassword.password, packet.tune2, sizeof(packet.tune2));
    command = packet.target_component;                                          // IT WILL BE PACKET.COMMAND

    if (command == ADD_PILOT){ 
        gcs().send_text(MAV_SEVERITY_WARNING, "ERROR: cant add pilot");
        add_pilot(_loginPassword);
    } else if (command == CHECK_PILOT)
    {
       check_pilot_password(_loginPassword);
    } else if (command == DELETE_PILOT)
    {
        /* code */
    } else {
        gcs().send_text(MAV_SEVERITY_WARNING, "ERROR: cant read command, unexpected value");
        return;
    }
}
//get mask
void  AP_UVRLoginAccess::get_pilot_mask(){
        // bool res;
        //         res = pilotMask.get(0);
        // gcs().send_text(MAV_SEVERITY_WARNING, "MASK %u",res);
    for(int i=0;i<MAX_PILOT_NUMBER;i++){
        bool res;
        res = pilotMask.get(i);
        gcs().send_text(MAV_SEVERITY_WARNING, "MASK %u",res);

                //     res = _storage.read_block(data,LOGIN_DATA_MAGIC_NUMBER,sizeof(struct loginPassword));
                // if(!str_compare(data,0)){
                // mask | i;
                // }
                // if (!res){
                //     gcs().send_text(MAV_SEVERITY_WARNING, "ERROR: cant read storage");
                //     return false;
                // }
    }
}

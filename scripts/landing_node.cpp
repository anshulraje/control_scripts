#include "control_scripts/landing_node.hpp"

mavros_msgs::State current_state_land;
geometry_msgs::PoseStamped pose_land;

Land::Land(int id){
    string uav = "uav";
    string uav_id = to_string(id);

    string state_sub_topic = "/mavros/state";
    this->state_sub = this->nh.subscribe(state_sub_topic, 10, &Land::state_cb, this);

    string landing_client_topic = "/mavros/cmd/land";
    this->landing_client = this->nh.serviceClient<mavros_msgs::CommandTOL>(landing_client_topic);
}

void Land::state_cb(const mavros_msgs::State& msg){
    current_state_land = msg;
}

void Land::init_connection(){
    Rate rate(20);
    ROS_INFO("Connecting to FCT...");
    while(ok() && current_state_land.connected){
        ROS_INFO("Connecting to FCT...");
        spinOnce();
        rate.sleep();
    }
    ROS_INFO("Connected!");
}

void Land::land(){
    Rate rate(20);
    
    mavros_msgs::CommandTOL landing_cmd;
    landing_cmd.request.latitude = nan("1");
    landing_cmd.request.longitude = nan("1");

    Time last_request = Time::now();

    bool flag = true;

    while(flag){
        if(current_state_land.mode != "AUTO.LAND" && (Time::now() - last_request > Duration(5.0))){
            if(landing_client.call(landing_cmd) && landing_cmd.response.success){
                ROS_INFO("Landing");
                flag = false;
            }
            last_request = Time::now();
        } 
        spinOnce();
        rate.sleep();
    }
}

//temp
int main(int argc, char **argv){
    init(argc, argv, "landing_node", init_options::AnonymousName);
    int uav_id = 0;

    Land land(uav_id);
    land.init_connection();
    land.land();
}
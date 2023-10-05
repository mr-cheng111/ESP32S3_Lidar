#include <Arduino.h>
#include <micro_ros_platformio.h>
#include <WiFi.h>
#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <std_msgs/msg/int32.h>
#include <string>
#include <Ticker.h>

#include <sensor_msgs/msg/laser_scan.h>
#include <micro_ros_utilities/string_utilities.h>


using namespace std;

#define LED_Pin 4

class lidar_t
{
public:
    lidar_t(String Node_Name,
            char* Wifi_SSID,
            char* Pass_Word,
            String Host_Ip,
            uint16_t Port,
            int Serial_Num);

    lidar_t(uint8_t Serial_Num);

    static void callback_subscription_(const void *msgin);

    void Node_Work_Start(void)
    {
        delay(100);
        rclc_executor_spin_some(&this->executor, RCL_MS_TO_NS(100));
    }

    void Micro_Ros_Task()
    {
        digitalWrite(LED_Pin, HIGH);
        while(true)
        {
            this->Node_Work_Start();
        }
    }

    void Lidar_Data_Task()
    {
        while(true)
        {
            if(!(this->Lidar_Serial->available() % 58))
            {
                this->Lidar_Serial->read(this->Lidar_Rx_Buffer,58);
            }
            delay(1);
        }
    }

    void Ros_Init(String Node_Name);

    void Wifi_Init( char* Wifi_SSID,
                    char* Pass_Word,
                    String Host_Ip,
                    uint16_t Port,
                    uint16_t Wait_Time);
                    
    void Ros_Serial_Init(int Serial_Num);

    void Ros_Serial_Init(int Serial_Num,int Baud);

    static void Usart_Callback_(void)
    {
        // if()




    }
public:
    Ticker Tick;
    HardwareSerial *Lidar_Serial;
private:
    rclc_executor_t executor;
    rclc_support_t support;
    rcl_allocator_t allocator;
    rcl_node_t node;
    // 声明话题订阅者
    rcl_subscription_t subscriber;

    //声明话题发布者
    rcl_publisher_t publisher;           // 声明话题发布者
    sensor_msgs__msg__LaserScan pub_msg; // 声明消息文件

    // 声明消息文件
    std_msgs__msg__Int32 sub_msg;
    uint8_t Lidar_Rx_Buffer[58*4];
    
};
#include <Arduino.h>
#include <micro_ros_platformio.h>
#include <WiFi.h>
#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <std_msgs/msg/int32.h>
#include <string>
#include <Ticker.h>

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
            uint8_t Serial_Num);

    lidar_t(uint8_t Serial_Num);

    static void callback_subscription_(const void *msgin)
    {
        const std_msgs__msg__Int32 *msg = (const std_msgs__msg__Int32 *)msgin;
        if(msg->data == 0)
        {
            digitalWrite(LED_Pin, LOW);
        }
        else
        {
            digitalWrite(LED_Pin, HIGH);
        }
    }
    void Node_Work_Start(void)
    {
        delay(100);
        rclc_executor_spin_some(&this->executor, RCL_MS_TO_NS(100));
    }

    void Micro_Ros_Task()
    {
        while(true)
        {
            this->Node_Work_Start();
        }
    }

    void lidar_Data_Task()
    {
        while(true)
        {
            this->Lidar_Serial->printf("Hello World\r\n");
            delay(10);
        }
    }

    void Ros_Init(String Node_Name);

    void Wifi_Init( char* Wifi_SSID,
                    char* Pass_Word,
                    String Host_Ip,
                    uint16_t Port,
                    uint16_t Wait_Time);
                    
    void Ros_Serial_Init(int Serial_Num)
    {
        switch(Serial_Num)
        {
            case 0:this->Lidar_Serial = &Serial;break;
            case 1:this->Lidar_Serial = &Serial1;break;
            case 2:this->Lidar_Serial = &Serial2;break;
            default :this->Lidar_Serial = &Serial;break;
        }
        // this->Lidar_Serial->setPins(15,16,-1,-1);
        // this->Lidar_Serial->setRxBufferSize(57);
        // this->Lidar_Serial->setTxBufferSize(1024);
        this->Lidar_Serial->begin(115200);
        // this->Lidar_Serial->setPins(15,16);

        //this->Lidar_Serial->onReceive(this->Usart_Callback_,0);
    }

    static void Usart_Callback_(void)
    {
        // if()




    }





    // void Ticker_Init()
    // {
    //     this->Tick.attach
    // }
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
    // 声明消息文件
    std_msgs__msg__Int32 sub_msg;
    
};



void lidar_a_Task(void *p);
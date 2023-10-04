#include <Arduino.h>
#include <micro_ros_platformio.h>
#include <WiFi.h>
#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <std_msgs/msg/int32.h>
#include <string>


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
            uint8_t Serial_Num)
    {
        Ros_Serial_Init(Serial_Num);

        Wifi_Init(Wifi_SSID,Pass_Word,Host_Ip,Port,2000);

        Ros_Init(Node_Name);

        // 初始化LED
        pinMode(LED_Pin, OUTPUT);

        digitalWrite(LED_Pin, HIGH);

        /**
         * @brief 创建一个任务在Core 0 上
         * microros_task    任务函数
         * "microros_task"  任务名称
         * 10240      任务占用内存大小
         * NULL         任务参数，为空
         * 1               任务优先级
         * NULL     任务Handle可以为空
         * 0                 内核编号
         */
        xTaskCreatePinnedToCore([](void*param)->void
                                {lidar_t *I = static_cast<lidar_t*>(param);
                                 I->Micro_Ros_Task();},
                                "microros_task", 
                                1024, 
                                this, 
                                1, 
                                NULL, 
                                0);
                /**
         * @brief 创建一个任务在Core 0 上
         * lidar_Data_Task    任务函数
         * "lidar_Data_Task"  任务名称
         * 1024      任务占用内存大小
         * this         任务参数，为空
         * 1               任务优先级
         * NULL     任务Handle可以为空
         * 0                 内核编号
         */
        xTaskCreatePinnedToCore([](void*param)->void
                                {lidar_t *I = static_cast<lidar_t*>(param);
                                 I->lidar_Data_Task();},
                                "lidar_Data_Task", 
                                1024, 
                                this, 
                                1, 
                                NULL, 
                                0);
    }

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
            if()

        }
    }

    void Ros_Init(String Node_Name)
    {
        // 初始化内存分配器
        this->allocator = rcl_get_default_allocator();
        // 创建初始化选项
        rclc_support_init(&this->support, 0, NULL, &this->allocator);
        // 创建节点 microros_wifi
        rclc_node_init_default(&this->node, "microros_wifi", "", &this->support);
        //定义一个订阅者，并将他的名称定义为led_control
        rclc_subscription_init_default(
            &this->subscriber,
            &this->node,
            ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
            Node_Name.c_str());
        
        // 创建执行器
        rclc_executor_init(&this->executor, &this->support.context, 1, &this->allocator);
        // 为执行器添加一个订阅者
        rclc_executor_add_subscription(&this->executor, &this->subscriber, &this->sub_msg, &lidar_t::callback_subscription_, ON_NEW_DATA);
    }

    void Wifi_Init( char* Wifi_SSID,
                    char* Pass_Word,
                    String Host_Ip,
                    uint16_t Port,
                    uint16_t Wait_Time)
    {
        // 设置通过WIFI进行MicroROS通信
        IPAddress agent_ip;
        agent_ip.fromString(Host_Ip.c_str());
        // 设置wifi名称，密码，电脑IP,端口号
        set_microros_wifi_transports(Wifi_SSID, Pass_Word, agent_ip, Port);
        delay(Wait_Time);
        
    }

    void Ros_Serial_Init(int Serial_Num)
    {
        switch(Serial_Num)
        {
            case 0:this->Lidar_Serial = &Serial;break;
            case 1:this->Lidar_Serial = &Serial1;break;
            case 2:this->Lidar_Serial = &Serial2;break;
            default :this->Lidar_Serial = &Serial;break;
        }
        this->Lidar_Serial->begin(230400);
        this->Lidar_Serial->setPins(15,16);
        this->Lidar_Serial->setRxBufferSize(57);
        this->Lidar_Serial->setTxBufferSize(1024);
        this->Lidar_Serial->onReceive(this->Usart_Callback_,0);
    }

    static void Usart_Callback_(void)
    {
        if(this)


    }
private:
    rclc_executor_t executor;
    rclc_support_t support;
    rcl_allocator_t allocator;
    rcl_node_t node;
    // 声明话题订阅者
    rcl_subscription_t subscriber;
    // 声明消息文件
    std_msgs__msg__Int32 sub_msg;
    HardwareSerial *Lidar_Serial;
};

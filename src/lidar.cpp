#include "../include/lidar.hpp"


void lidar_a_Task(void *p)
{
    while(1)
    {
        Serial.printf("Hello World\r\n");
        delay(10);
    }

}

lidar_t::lidar_t(String Node_Name,
                 char* Wifi_SSID,
                 char* Pass_Word,
                 String Host_Ip,
                 uint16_t Port,
                 int Serial_Num)
{
        Ros_Serial_Init(Serial_Num);

        Wifi_Init(Wifi_SSID,Pass_Word,Host_Ip,Port,2000);

        Ros_Init(Node_Name);

        // 初始化LED
        pinMode(LED_Pin, OUTPUT);

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
                                "Micro_Ros_Task", 
                                100*1024, 
                                this, 
                                1, 
                                NULL, 
                                0);
        /**
         * @brief 创建一个任务在Core 0 上
         * Lidar_Data_Task    任务函数
         * "Lidar_Data_Task"  任务名称
         * 1024      任务占用内存大小
         * this         任务参数，为空
         * 1               任务优先级
         * NULL     任务Handle可以为空
         * 0                 内核编号
         */
        xTaskCreatePinnedToCore([](void*param)->void
                                {lidar_t *I = static_cast<lidar_t*>(param);
                                 I->Lidar_Data_Task();},
                                "Lidar_Data_Task", 
                                2*1024, 
                                this, 
                                1, 
                                NULL, 
                                0);
}

lidar_t::lidar_t(uint8_t Serial_Num)
{
        Ros_Serial_Init(Serial_Num);

        // 初始化LED
        //pinMode(LED_Pin, OUTPUT);

        //digitalWrite(LED_Pin, HIGH);
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
                                 I->Lidar_Data_Task();},
                                "Lidar_Data_Task", 
                                1536, 
                                this, 
                                1, 
                                NULL, 
                                0);
        // xTaskCreatePinnedToCore(lidar_a_Task,
        //                         "lidar_a_Task", 
        //                         10240, 
        //                         NULL, 
        //                         1, 
        //                         NULL, 
        //                         0);
}

void lidar_t::Ros_Init(String Node_Name)
{
    // 初始化内存分配器
    this->allocator = rcl_get_default_allocator();
    // 创建初始化选项
    rclc_support_init(&this->support, 0, NULL, &this->allocator);
    // 创建节点 microros_wifi
    rclc_node_init_default(&this->node, "microros_wifi", "", &this->support);
    //定义一个订阅者，并将他的名称定义为Node_Name
    // rclc_subscription_init_default(
    //     &this->subscriber,
    //     &this->node,
    //     ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
    //     Node_Name.c_str());
    //定义一个话题发布者
    rclc_publisher_init_default(
        &this->publisher,
        &this->node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, LaserScan),
        Node_Name.c_str());
        
    // 创建执行器
    rclc_executor_init(&this->executor, &this->support.context, 1, &this->allocator);
    // 为执行器添加一个订阅者
    rclc_executor_add_subscription(&this->executor, &this->subscriber, &this->sub_msg, &lidar_t::callback_subscription_, ON_NEW_DATA);
}

void lidar_t::Wifi_Init(char* Wifi_SSID,
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

void lidar_t::Ros_Serial_Init(int Serial_Num,int Baud)
{
    switch(Serial_Num)
    {
        case 0:this->Lidar_Serial = &Serial;break;
        case 1:this->Lidar_Serial = &Serial1;break;
        case 2:this->Lidar_Serial = &Serial2;break;
        default :this->Lidar_Serial = &Serial;break;
    }
    //this->Lidar_Serial->setPins(15,16,-1,-1);
    this->Lidar_Serial->setRxBufferSize(58);
    this->Lidar_Serial->setTxBufferSize(1024);
    this->Lidar_Serial->begin(Baud);
        // this->Lidar_Serial->setPins(15,16);

        //this->Lidar_Serial->onReceive(this->Usart_Callback_,0);
}

void lidar_t::Ros_Serial_Init(int Serial_Num)
{
    switch(Serial_Num)
    {
        case 0:this->Lidar_Serial = &Serial;break;
        case 1:this->Lidar_Serial = &Serial1;break;
        case 2:this->Lidar_Serial = &Serial2;break;
        default :this->Lidar_Serial = &Serial;break;
    }
    //this->Lidar_Serial->setPins(15,16,-1,-1);
    this->Lidar_Serial->setRxBufferSize(58);
    this->Lidar_Serial->setTxBufferSize(1024);
    this->Lidar_Serial->begin(115200);
        // this->Lidar_Serial->setPins(15,16);

        //this->Lidar_Serial->onReceive(this->Usart_Callback_,0);
}

void lidar_t::callback_subscription_(const void *msgin)
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

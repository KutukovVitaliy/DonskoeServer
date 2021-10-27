//
// Created by kvitadmin on 26.10.2021.
//

#include "MqttModbus.h"

void MqttModbus::reconnect() {
    std::this_thread::sleep_for(std::chrono::milliseconds(2500));
    try {
        cli_.connect(connOpts_, nullptr, *this);
    }
    catch (const mqtt::exception& exc) {
        std::cerr << "Error: " << exc.what() << std::endl;
        exit(1);
    }
}

void MqttModbus::on_failure(const mqtt::token& tok){
    std::cout << "Connection attempt failed" << std::endl;
    if (++nretry_ > N_RETRY_ATTEMPTS)
        exit(1);
    reconnect();
}

void MqttModbus::connected(const std::string& cause){
    std::cout << "\nConnection success" << std::endl;
    std::cout << "\nSubscribing to topic '" << TOPIC_S << "'\n"
    << "\tfor client " << CLIENT_ID
    << " using QoS" << QOS << "\n"
    << "\nPress Q<Enter> to quit\n" << std::endl;
    cli_.subscribe(topicReceive, QOS, nullptr, subListener_);
}

void MqttModbus::connection_lost(const std::string& cause){
    std::cout << "\nConnection lost" << std::endl;
    if (!cause.empty())
        std::cout << "\tcause: " << cause << std::endl;

    std::cout << "Reconnecting..." << std::endl;
    nretry_ = 0;
    reconnect();
}
void MqttModbus::message_arrived(mqtt::const_message_ptr msg){

    std::cout << "Message arrived" << std::endl;
    std::cout << "\ttopic: '" << msg->get_topic() << "'" << std::endl;
    std::cout << "\tpayload: '" << msg->to_string() << "'\n" << std::endl;
    std::string message = msg->to_string();
    std::vector<uint8_t> resp = MessageToRtu(message);
    if(!resp.empty()){
        uint16_t CurCrc = (resp.at(resp.size()-2) << 8) | resp.at(resp.size() - 1);
        if(CurCrc == Crc16(resp, resp.size() - 2)){
            response = message;
            responseOk = true;
        }
    }

}
std::pair<bool, uint16_t> MqttModbus::ReadRegister(std::string SlaveAddress, std::string RegAddress, std::string nb){
    std::string MessageToSend;
    if(SlaveAddress.size() != 4 || RegAddress.size() != 4 || nb.size() != 4){
        return std::make_pair(false, 0);
    }
    if(std::stoi(SlaveAddress) > 255 || std::stoi(SlaveAddress) < 1){
        return std::make_pair(false, 0);
    }
    MessageToSend = SlaveAddress + "03" + RegAddress + nb;
    std::vector<uint8_t> mes = MessageToRtu(MessageToSend);
    uint16_t crc = Crc16(mes, mes.size());
    uint8_t HiCrc = static_cast<uint8_t>(crc >> 8);
    uint8_t LoCrc = static_cast<uint8_t>(crc & 0x00FF);
    mes.push_back(HiCrc);
    mes.push_back(LoCrc);
}
std::string MqttModbus::ModbusToString(int resp_length){
    std::string resp;
    for(int i = 0; i < resp_length; ++i){
        //resp = resp + std::string(1,HexToAscii((rsp[i] >> 4) & 0x0F)) + std::string(1, HexToAscii(rsp[i] & 0x0F));
    }
    return resp;
}
char MqttModbus::HexToAscii(char hex_char){
    if(hex_char >= 0 && hex_char <= 9) hex_char += 48;
    else if(hex_char >= 0xA && hex_char <= 0x0F) hex_char += 55;
    return hex_char;
}

uint8_t MqttModbus::AsciiToHex(char ascii_char){
    if(ascii_char >= 48 && ascii_char <= 57) ascii_char -= 48;
    else if(ascii_char >= 65 && ascii_char <= 70) ascii_char -= 55;
    return ascii_char;
}

std::vector<uint8_t> MqttModbus::MessageToRtu(std::string& message){
    std::vector<uint8_t> rtuMessage;
    if((message.size() % 2) == 0){
        for(int i = 0; i < message.size(); i+=2){
            rtuMessage.push_back(((AsciiToHex(message.at(i)) << 4) & 0xF0) | (AsciiToHex(message.at(i+1)) & 0x0F));

        }
    }

    return rtuMessage;

}
MqttModbus::~MqttModbus(){
    /*if(m_portState) {
        DisconnectFromPort();
        modbus_free(m_modbus);
    }*/
}

uint16_t MqttModbus::Crc16(std::vector<uint8_t>& buffer, int bufferLength){
    uint8_t crc_hi = 0xFF; /* high CRC byte initialized */
    uint8_t crc_lo = 0xFF; /* low CRC byte initialized */
    unsigned int i; /* will index into CRC lookup */

    /* pass through message buffer */
    for (int k = 0; k < bufferLength; ++k) {
        i = crc_hi ^ buffer.at(k); /* calculate the CRC  */
        crc_hi = crc_lo ^ table_crc_hi[i];
        crc_lo = table_crc_lo[i];
    }

    return (crc_hi << 8 | crc_lo);
}
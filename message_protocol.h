// message_protocol.h
#pragma once
#include <string>
#include <chrono>

namespace DrawSync {

    // 消息类型枚举
    enum class MessageType {
        UNKNOWN = 0,
        PLAYER_JOIN = 1,
        PLAYER_LEAVE = 2,
        DRAW_DATA = 3,
        CHAT_MESSAGE = 4,
        GUESS_WORD = 5,
        ROOM_CREATE = 10,
        ROOM_JOIN = 11,
        ROOM_LEAVE = 12,
        ROOM_LIST = 13,
        GAME_START = 20,      // 新增
        GAME_END = 21,        // 新增
        ROUND_START = 22,     // 新增
        ROUND_END = 23,       // 新增
        TIMER_UPDATE = 24,    // 新增
        SCORE_UPDATE = 25     // 新增
    };

    class MessageParser {
    public:
        // 解析消息类型
        static MessageType get_message_type(const std::string& message) {
            if (message.empty() || message[0] != '{')
                return MessageType::UNKNOWN;

            size_t type_pos = message.find("\"type\"");
            if (type_pos == std::string::npos)
                return MessageType::UNKNOWN;

            size_t value_pos = message.find(':', type_pos);
            if (value_pos == std::string::npos)
                return MessageType::UNKNOWN;

            size_t start = message.find_first_of("1234567890", value_pos);
            if (start == std::string::npos)
                return MessageType::UNKNOWN;

            size_t end = message.find_first_not_of("1234567890", start);
            std::string type_str = message.substr(start, end - start);

            try {
                return static_cast<MessageType>(std::stoi(type_str));
            }
            catch (...) {
                return MessageType::UNKNOWN;
            }
        }

        // 创建响应消息（手动构建JSON字符串）
        static std::string create_response(MessageType type, const std::string& status) {
            return "{\"response_type\":" + std::to_string(static_cast<int>(type)) +
                ",\"status\":\"" + status + "\"," +
                "\"timestamp\":" + std::to_string(get_timestamp()) + "}";
        }

        // 创建房间创建消息
        static std::string create_room_create(const std::string& room_id, const std::string& room_name) {
            return "{\"type\":10,\"room_id\":\"" + room_id +
                "\",\"room_name\":\"" + room_name +
                "\",\"timestamp\":" + std::to_string(get_timestamp()) + "}";
        }

        // 创建房间加入消息
        static std::string create_room_join(const std::string& room_id, const std::string& player_id) {
            return "{\"type\":11,\"room_id\":\"" + room_id +
                "\",\"player_id\":\"" + player_id +
                "\",\"timestamp\":" + std::to_string(get_timestamp()) + "}";
        }

    private:
        static long long get_timestamp() {
            return std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count();
        }
    };

} // namespace DrawSync
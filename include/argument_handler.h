#ifndef ELTECAR_DATASERVER_INCLUDE_ARGUMENT_HANDLER_H
#define ELTECAR_DATASERVER_INCLUDE_ARGUMENT_HANDLER_H

#include <string>
#include <vector>
class ArgumentHandler {
public:
    static void DelayHandler(std::string input);
    static void BaseImageHandler(std::vector<std::string> inputs);
    static void DataPointHandler(std::string input);
    static void CsvHandler(std::string input);
    static void LidarHandler(std::string input);

    static inline std::vector<std::string> m_imageNames;
    static inline std::string m_csvPath;
    static inline std::string m_lidarPath;
    static inline int m_delay;
    static inline unsigned int m_numberOfDataPoints;
};

#endif// ELTECAR_DATASERVER_INCLUDE_ARGUMENT_HANDLER_H

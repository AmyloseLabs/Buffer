#ifndef BUFFER_H
#define BUFFER_H

/**
 * @file buffer.h
 * @brief class Buffer Definition/Implementation header file.
 */

#include <deque>
#include <vector>
#include <mutex>

/**
 * @brief reverse helper for buffer access
 */
template <typename T>
struct reversion_wrapper { T& iterable; };

/**
 * @brief reverse helper for buffer access
 */
template <typename T>
auto begin (reversion_wrapper<T> w) { return std::rbegin(w.iterable); }

/**
 * @brief reverse helper for buffer access
 */
template <typename T>
auto end (reversion_wrapper<T> w) { return std::rend(w.iterable); }

/**
 * @brief reverse helper for buffer access
 */
template <typename T>
reversion_wrapper<T> reverse (T&& iterable) { return { iterable }; }


/**
 * @brief buffer access direction control code index
 */
enum class PushPopType {
    Front = 0x00,
    Rear = 0xFF
};

/**
 * @class Buffer
 * @brief 버퍼 정의 클래스
 * @tparam _Typ : Data type for use
 * @details packet을 임시저장하거나, 작업순서를 임시저장할 수 있음
 */
template<typename _Typ>
class Buffer
{
private:
    PushPopType m_ePushType;
    PushPopType m_ePopType;

    std::mutex *m_bufMutex;

    std::deque<_Typ> m_buffer;

public:
    Buffer(PushPopType ePushTyp = PushPopType::Rear,
           PushPopType ePopTyp = PushPopType::Front) : m_bufMutex(nullptr){
        m_ePushType = ePushTyp;
        m_ePopType = ePopTyp;
        m_buffer.resize(0);

        m_bufMutex = new std::mutex;

    }

    virtual ~Buffer() {
        m_buffer.clear();
        if (m_bufMutex) {
            delete m_bufMutex;
            m_bufMutex = nullptr;
        }
    }

    /**
     * @brief push : push data to buffer;
     * @param[in] data : _Typ &data;
     */
    void push (_Typ &data) {
        std::lock_guard<std::mutex> lock(*m_bufMutex);
        switch (m_ePushType) {
            case PushPopType::Front:
                m_buffer.emplace_front(data);
                break;
            case PushPopType::Rear:
                m_buffer.emplace_back(data);
                break;
        }
    }

    /**
     * @brief push : Push data List (Mulit) to Buffer (overload)
     * @param[in] dataList : std::vector<_Typ> &dataList
     */
    void push (std::vector<_Typ> &dataList) {
        std::lock_guard<std::mutex> lock(*m_bufMutex);
        for (auto &data : dataList){
            switch (m_ePushType) {
                case PushPopType::Front:
                    m_buffer.emplace_front(data);
                    break;
                case PushPopType::Rear:
                    m_buffer.emplace_back(data);
                    break;
            }
        }
    }

    /**
     * @brief pop : pop data from buffer;
     * @param[out] data : _Typ &data
     */
    void pop (_Typ &data) {
        std::lock_guard<std::mutex> lock(*m_bufMutex);
        if (!m_buffer.empty()) {
            switch (m_ePopType) {
                case PushPopType::Front:
                    data = m_buffer.front();
                    m_buffer.pop_front();
                    break;
                case PushPopType::Rear:
                    data = m_buffer.back();
                    m_buffer.pop_back();
                    break;
            }
        }
    }

    /**
     * @brief pop : pop All Data from Buffer;
     * @param[out] dataList : std::vector<_Typ> &dataList;
     * @details When this function called, All data from buffer can extracted to vector.
     * @n       After this function called, buffer will be empty.
     */
    void pop (std::vector<_Typ> &dataList) {
        std::lock_guard<std::mutex> lock(*m_bufMutex);
        if (!m_buffer.empty()) {
            switch (m_ePopType) {
                case PushPopType::Front:
                    for (auto &parts : m_buffer) {
                        dataList.emplace_back(parts);
                    }
                    break;
                case PushPopType::Rear:
                    for (auto &parts : reverse(m_buffer)) {
                        dataList.emplace_back(parts);
                    }
                    break;
            }
            m_buffer.clear();
        }
    }

    /**
     * @brief size : get Current buffer size;
     * @return (int)current buffer size
     */
    size_t size(void) {
        size_t nSz = 0;
        std::lock_guard<std::mutex> lock(*m_bufMutex);
        nSz = m_buffer.size();
        return nSz;
    }
};

#endif // BUFFER_H


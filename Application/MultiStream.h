#pragma once

#include <functional>
#include <iosfwd>

class MultiStreamBuffer : public std::streambuf {
public:
    void AddStream(std::ostream& os) {
        m_Streams.push_back(&os);
    }

protected:
    Int overflow(Int c) override {
        if (c == EOF)
            return EOF;

        for (auto* s : m_Streams)
            s->put(static_cast<Char>(c));
        return c;
    }
    std::streamsize xsputn(const Char* s, std::streamsize n) override {
        for (auto* out : m_Streams)
            out->write(s, n);
        return n;
    }

private:
    std::vector<std::ostream*> m_Streams;
};


class MultiStream : public std::ostream {
public:
    MultiStream() :
        std::ostream(&m_Buffer)
	{
	}

    void AddStream(std::ostream& stream) {
        m_Buffer.AddStream(stream);
    }

private:
    MultiStreamBuffer m_Buffer;
};
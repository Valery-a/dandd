#ifndef FRAMEBUFFER_EXCEPTION_HPP
#define FRAMEBUFFER_EXCEPTION_HPP

#include <exception>
#include <string>

class FramebufferException : public std::exception {
  public:
    FramebufferException();
    const char* what() const noexcept override;

  private:
    std::string m_message;
};

#endif // FRAMEBUFFER_EXCEPTION_HPP

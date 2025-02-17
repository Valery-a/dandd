#ifndef IMAGE_EXCEPTION_HPP
#define IMAGE_EXCEPTION_HPP

#include <exception>
#include <string>

class ImageException : public std::exception {
  public:
    ImageException();
    const char* what() const noexcept override;

  private:
    std::string m_message;
};

#endif // IMAGE_EXCEPTION_HPP

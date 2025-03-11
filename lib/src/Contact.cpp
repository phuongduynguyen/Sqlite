#include <Contact.h>

Contact::Contact(const std::string& name,const std::vector<std::string>& numbers,const std::string& notes, const std::string& uri) : mName(name),
                                                                                                                                     mNumbers(numbers),
                                                                                                                                     mNotes(notes),
                                                                                                                                     mUri(uri)
{

}

std::string Contact::getName() const
{
    return mName;
}

std::vector<std::string> Contact::getPhoneNumbers() const
{
    return mNumbers;
}

std::string Contact::getNotes() const
{
    return mNotes;
}

std::string Contact::getUri() const
{
    return mUri;
}

std::vector<unsigned char> Contact::getblobImage() const
{
    return mImages;
}

void Contact::showImage()
{
    // TBD: Need using OpenGL to render
}

std::string Contact::toString() const {
    // TBD: Need phasing to string which insert in database
    return "";
}

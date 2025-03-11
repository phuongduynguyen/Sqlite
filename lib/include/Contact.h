#ifndef CONTACT_H
#define CONTACT_H
#include <iostream>
#include <string>
#include <vector>

class Contact final
{
    public:
        class Builder
        {
            private:
                std::string mName;
                std::vector<std::string> mNumbers;
                std::string mNotes;
                std::string mUri;
   
            public:
                Builder(const Contact& contact) {
                    mName = contact.getName();
                    mNumbers = contact.getPhoneNumbers();
                    mNotes = contact.getNotes();
                    mUri = contact.getUri();
                }

                Builder& setName(const std::string& name) {
                    mName = name;
                    return *this;
                }

                Builder& setPhoneNumbers(const std::vector<std::string>& numbers) {
                    mNumbers = numbers;
                    return *this;
                }

                Builder& setNotes(const std::string& notes) {
                    mNotes = notes;
                    return *this;
                }

                Builder& setImageUri(const std::string& uri) {
                    mUri = uri;
                    return *this;
                }

                Contact build() {
                    return Contact(mName, mNumbers, mNotes, mUri);
                }
        };

        std::string getName() const;
        std::vector<std::string> getPhoneNumbers() const;
        std::string getNotes() const;
        std::string getUri() const;
        std::vector<unsigned char>  getblobImage() const;
        void showImage();
        std::string toString() const;
    private:
        Contact(const std::string& name,const std::vector<std::string>& numbers,const std::string& notes, const std::string& uri);

        std::string mName;
        std::vector<std::string> mNumbers;
        std::string mNotes;
        std::string mUri;
        std::vector<unsigned char> mImages;
};

#endif // CONTACT_H
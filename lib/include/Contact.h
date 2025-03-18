#ifndef CONTACT_H
#define CONTACT_H
#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <sstream>
#include <memory>
#include <jpeglib.h>

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
                std::vector<unsigned char> mBlob;
            public:
                Builder(const Contact& contact) {
                    mName = contact.getName();
                    mNumbers = contact.getPhoneNumbers();
                    mNotes = contact.getNotes();
                    mUri = contact.getUri();
                }

                Builder() {

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

                Builder& setBlobImage(const std::vector<unsigned char>& blob) {
                    mBlob = blob;
                    return *this;
                }

                Contact build() {
                    return Contact(mName, mNumbers, mNotes, mUri);
                }

                std::shared_ptr<Contact> buildShared() {
                    return std::shared_ptr<Contact>(new Contact(mName, mNumbers, mNotes, mUri));
                }

                std::unique_ptr<Contact> buildUnique() {
                    return std::unique_ptr<Contact>(new Contact(mName, mNumbers, mNotes,mUri));
                }
        };

        std::string getName() const;
        std::vector<std::string> getPhoneNumbers() const;
        std::string getPhoneNumbersString() const;
        std::string getNotes() const;
        std::string getUri() const;
        std::vector<unsigned char>  getblobImage() const;
        int getId() const;
        void showImage();
        std::string toString() const;
        void setName(const std::string& name);
        void setPhoneNumbers(const std::string& phoneNums);
        void setNotes(const std::string& notes);
        void setUri(const std::string& uri);
        void setBlobImage(const std::vector<unsigned char>& blob);
        void setId(const int& id);
        bool operator==(const Contact& other);

        template<typename T>
        void syncContact(T&& other);

    private:
        Contact(const std::string& name,const std::vector<std::string>& numbers,const std::string& notes, const std::string& uri, const std::vector<unsigned char>& blob = std::vector<unsigned char>{});

        std::string mName;
        std::vector<std::string> mNumbers;
        std::string mNotes;
        std::string mUri;
        std::vector<unsigned char> mImagesBlob;
        int mId;
};

#endif // CONTACT_H
#include <Contact.h>
#include <cerrno>
#include <cstring>
Contact::Contact(const std::string& name,const std::vector<std::string>& numbers,const std::string& notes, const std::string& uri, const std::vector<unsigned char>& blob) : mName(name),
                                                                                                                                     mNumbers(numbers),
                                                                                                                                     mNotes(notes),
                                                                                                                                     mUri(uri),
                                                                                                                                     mImagesBlob(blob)
{
    std::FILE* file = std::fopen(mUri.c_str(), "rb");
    if (file) {
        struct jpeg_decompress_struct cinfo;
        struct jpeg_error_mgr jerr;
        cinfo.err = jpeg_std_error(&jerr);
        jpeg_create_decompress(&cinfo);
        jpeg_stdio_src(&cinfo, file);
        jpeg_read_header(&cinfo, TRUE);
        jpeg_start_decompress(&cinfo);
    
        int rowStride = cinfo.output_width * cinfo.output_components;
        mImagesBlob.resize(cinfo.output_height * rowStride);
        while (cinfo.output_scanline < cinfo.output_height) {
            unsigned char* row = mImagesBlob.data() + (cinfo.output_scanline * rowStride);
            jpeg_read_scanlines(&cinfo, &row, 1);
        }
    
        jpeg_finish_decompress(&cinfo);
        jpeg_destroy_decompress(&cinfo);
        std::fclose(file);   
    }
}


std::string Contact::getName() const
{
    return mName;
}

std::vector<std::string> Contact::getPhoneNumbers() const
{
    return mNumbers;
}

std::string Contact::getPhoneNumbersString() const
{
    std::ostringstream oss;
    switch (mNumbers.size())
    {
        case 0: {
            break;
        }
        case 1: {
            oss << mNumbers[0];
            break;
        }
        default: {
            oss << mNumbers[0];
            for (size_t i = 1; i < mNumbers.size(); i++) {
                oss << "," << mNumbers[i];
            }
            break;
        }
    }
    return oss.str();
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
    return mImagesBlob;
}

int Contact::getId() const
{
    return mId;
}

void Contact::showImage()
{
    // TBD: Need using OpenGL to render
}

std::string Contact::toString() const {
    return "Contact - name: " + mName + " number: " + getPhoneNumbersString() + " Notes: " + mNotes + " id: " + std::to_string(mId).c_str() + "\n";
}

void Contact::setName(const std::string& name)
{
    mName = name;
}

void Contact::setPhoneNumbers(const std::string& phoneNums)
{
    // Todo: Separate list phone numbers to list
    mNumbers.emplace_back(phoneNums);
}

void Contact::setNotes(const std::string& notes)
{
    mNotes = notes;
}

void Contact::setUri(const std::string& uri)
{
    mUri = uri;
}

void Contact::setBlobImage(const std::vector<unsigned char>& blob)
{
    mImagesBlob = blob;
}

void Contact::setId(const int& id)
{
    mId = id;
}

bool Contact::operator==(const Contact& other)
{
    return ((mId == other.getId()) && (mName == other.getName()) && (mNumbers == other.getPhoneNumbers()) && (mNotes == other.getNotes()));
}

bool Contact::equal(const Contact& other)
{
    std::cout << "id: " << ((mId == other.getId())) << " name: " << (mName == other.getName()) << " numbers: " << (mNumbers == other.getPhoneNumbers())  << " note: " << (mNotes == other.getNotes()) << "\n"; 
    return ((mId == other.getId()) && (mName == other.getName()) && (mNumbers == other.getPhoneNumbers()) && (mNotes == other.getNotes()));
}

template<typename T>
void Contact::syncContact(T&& other)
{
    if constexpr (std::is_same_v<std::decay_t<T>, Contact>) {
        std::cout << "Syncing with const Contact&\n";
        mName = other.getName();
        mId = other.getId();
        mNumbers = other.getPhoneNumbers();
        mNotes = other.getNotes();
        mUri = other.getUri();
        mImagesBlob = other.getblobImage();
    }
    else if constexpr(std::is_same_v<std::decay_t<T>, std::shared_ptr<Contact>>) {
        std::cout << "Syncing with shared_ptr Contact\n";
        if (other) {
            mName = other->getName();
            mId = other->getId();
            mNumbers = other->getPhoneNumbers();
            mNotes = other->getNotes();
            mUri = other->getUri();
            mImagesBlob = other->getblobImage();
        }
        else {
            std::wcerr << "syncContact but null ptr\n";
        }
    }
    else if (std::is_same_v<std::remove_reference_t<T>,const std::unique_ptr<Contact>>)
    {
        std::cout << "Syncing with unique_ptr Contact\n";
        if (other) {
            mName = other->getName();
            mId = other->getId();
            mNumbers = other->getPhoneNumbers();
            mNotes = other->getNotes();
            mUri = other->getUri();
            mImagesBlob = other->getblobImage();
        }
        else {
            std::wcerr << "syncContact but null ptr\n";
        }
    }
    else {
        std::wcerr << "syncContact Not match any type\n";
    }
}

template void Contact::syncContact<const Contact&>(const Contact&);
template void Contact::syncContact<std::unique_ptr<Contact>>(std::unique_ptr<Contact>&&);
template void Contact::syncContact<std::shared_ptr<Contact>>(std::shared_ptr<Contact>&&);
template void Contact::syncContact<std::shared_ptr<Contact>&>(std::shared_ptr<Contact>&);

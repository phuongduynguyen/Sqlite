#include <Contact.h>

Contact::Contact(const std::string& name,const std::vector<std::string>& numbers,const std::string& notes, const std::string& uri) : mName(name),
                                                                                                                                     mNumbers(numbers),
                                                                                                                                     mNotes(notes),
                                                                                                                                     mUri(uri)
{
    std::FILE* file = std::fopen(mUri.c_str(), "rb");
    if (!file) {
        throw std::runtime_error("Cannot open JPEG file: " + mUri);
    }

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
    if(!mNumbers.empty()) {
        oss << mNumbers[0];
        for (size_t i = 0; i < mNumbers.size(); i++) {
            oss << ';' << mNumbers[i];
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

void Contact::showImage()
{
    // TBD: Need using OpenGL to render
}

std::string Contact::toString() const {
    // TBD: Need phasing to string which insert in database
    return "";
}

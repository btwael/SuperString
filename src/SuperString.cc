#include "SuperString.hh"

/*-- imports --*/

// std
#include <iostream> // The only thing we need, and just for printing

/*-- definitions --*/

//*-- SuperString
SuperString::SuperString()
        : _sequence(NULL) {
    // nothing go here
}

SuperString::SuperString(const SuperString &other) /*copy*/ {
    this->_sequence = other._sequence;
    this->_sequence->refAdd();
}

SuperString::SuperString(SuperString::StringSequence *sequence)
        : _sequence(sequence) {
    this->_sequence->refAdd();
}

SuperString::~SuperString() {
    if(this->_sequence != NULL && this->_sequence->refRelease() == 0 && this->freeingCost() < this->keepingCost()) {
        delete this->_sequence;
    }
}

SuperString::Size SuperString::length() const {
    if(this->_sequence != NULL) {
        return this->_sequence->length();
    }
    return 0;
}

int SuperString::compareTo(const SuperString &other) const {
    Size thisLength = this->length();
    Size otherLength = other.length();
    Size len = (thisLength < otherLength) ? thisLength : otherLength;
    for(int i = 0; i < len; i++) {
        int thisCodeUnit = this->codeUnitAt(i).ok();
        int otherCodeUnit = other.codeUnitAt(i).ok();
        if(thisCodeUnit < otherCodeUnit) {
            return -1;
        }
        if(thisCodeUnit > otherCodeUnit) {
            return 1;
        }
    }
    if(thisLength < otherLength) return -1;
    if(thisLength > otherLength) return 1;
    return 0;
}

SuperString::Result<int, SuperString::Error> SuperString::codeUnitAt(SuperString::Size index) const {
    if(this->_sequence != NULL) {
        return this->_sequence->codeUnitAt(index);
    }
    return Result<int, SuperString::Error>(Error::Unexpected);
}

SuperString::Result<SuperString, SuperString::Error>
SuperString::substring(SuperString::Size startIndex, SuperString::Size endIndex) const {
    if(this->_sequence != NULL) {
        return this->_sequence->substring(startIndex, endIndex);
    }
    return Result<SuperString, Error>(Error::Unexpected);
}

void SuperString::print(std::ostream &stream) const {
    if(this->_sequence != NULL) {
        this->_sequence->print(stream);
    }
}

void SuperString::print(std::ostream &stream, SuperString::Size startIndex, SuperString::Size endIndex) const {
    if(this->_sequence != NULL) {
        this->_sequence->print(stream, startIndex, endIndex);
    }
}

SuperString SuperString::trim() const {
    if(this->_sequence != NULL) {
        return this->_sequence->trim();
    }
    return *this;
}

SuperString SuperString::trimLeft() const {
    if(this->_sequence != NULL) {
        return this->_sequence->trimLeft();
    }
    return *this;
}

SuperString SuperString::trimRight() const {
    if(this->_sequence != NULL) {
        return this->_sequence->trimRight();
    }
    return *this;
}

// TODO: delete this two methods
SuperString::Size SuperString::freeingCost() const {
    return this->_sequence->freeingCost();
}

SuperString::Size SuperString::keepingCost() const {
    return this->_sequence->keepingCost();
}

SuperString SuperString::operator+(const SuperString &other) const {
    ConcatenationSequence *sequence = new ConcatenationSequence(this->_sequence, other._sequence);
    return SuperString(sequence);
}

SuperString SuperString::operator*(Size times) const {
    MultipleSequence *sequence = new MultipleSequence(this->_sequence, times);
    return SuperString(sequence);
}

SuperString &SuperString::operator=(const SuperString &other) {
    if(this != &other) {
        if(this->_sequence->refRelease() == 0) {
            delete this->_sequence;
        }
        this->_sequence = other._sequence;
        this->_sequence->refAdd();
    }
    return *this;
}

SuperString::Bool SuperString::operator==(const SuperString &other) const {
    return this->compareTo(other) == 0;
}

SuperString SuperString::Const(const char *chars, SuperString::Encoding encoding) {
    StringSequence *sequence = NULL;
    switch(encoding) {
        case Encoding::ASCII:
            sequence = new SuperString::ConstASCIISequence(chars);
            break;
        case Encoding::UTF8:
            sequence = new SuperString::ConstUTF8Sequence(chars);
            break;
        case Encoding::UTF16:
            sequence = new SuperString::ConstUTF16Sequence((Byte *) chars);
            break;
    }
    return SuperString(sequence);
}

SuperString SuperString::Const(const SuperString::Byte *bytes, SuperString::Encoding encoding) {
    return SuperString::Const((char *) bytes, encoding);
}

SuperString SuperString::Copy(const char *chars, Encoding encoding) {
    StringSequence *sequence = NULL;
    switch(encoding) {
        case Encoding::ASCII:
            sequence = new SuperString::CopyASCIISequence(chars);
            break;
        case Encoding::UTF8:
            sequence = new SuperString::CopyUTF8Sequence(chars);
            break;
        case Encoding::UTF16:
            sequence = new SuperString::CopyUTF16Sequence((Byte *) chars);
            break;
    }
    return SuperString(sequence);
}

SuperString SuperString::Copy(const SuperString::Byte *bytes, Encoding encoding) {
    return SuperString::Copy((char *) bytes, encoding);
}

//*-- SuperString::StringSequence (abstract|internal)
SuperString::StringSequence::StringSequence()
        : _refCount(0) {
    // nothing go here
}

SuperString::StringSequence::~StringSequence() {
    SingleLinkedList<ReferenceStringSequence *>::Node<ReferenceStringSequence *> *node = this->_referencers._head;
    while(node != NULL) {
        node->_data->reconstruct(this);
        node = node->_next;
    }
}

SuperString::Bool SuperString::StringSequence::isEmpty() const {
    return this->length() == 0;
}

SuperString::Bool SuperString::StringSequence::isNotEmpty() const {
    return this->length() > 0;
}

void SuperString::StringSequence::refAdd() {
    this->_refCount++;
}

SuperString::Size SuperString::StringSequence::refRelease() {
    if(this->_refCount == 0) {
        return 0;
    }
    return --this->_refCount;
}

void SuperString::StringSequence::addReferencer(SuperString::ReferenceStringSequence *sequence) const {
    StringSequence *self = (StringSequence *) (unsigned long) this;
    self->_referencers.push(sequence);
}

void SuperString::StringSequence::removeReferencer(SuperString::ReferenceStringSequence *sequence) const {
    StringSequence *self = (StringSequence *) (unsigned long) this;
    self->_referencers.remove(sequence);
}

SuperString::Size SuperString::StringSequence::freeingCost() const {
    Size cost = 0;
    SingleLinkedList<ReferenceStringSequence *>::Node<ReferenceStringSequence *> *node = this->_referencers._head;
    while(node != NULL) {
        cost += node->_data->reconstructionCost();
        node = node->_next;
    }
    return cost;
}

//*-- SuperString::ReferenceStringSequence (abstract|internal)
SuperString::ReferenceStringSequence::~ReferenceStringSequence() {
    // nothing go here
}

//*-- SuperString::ConstASCIISequence (internal)
SuperString::ConstASCIISequence::ConstASCIISequence(const char *chars)
        : _chars(chars),
          _lengthComputed(SuperString::FALSE) {
    // nothing go here
}

SuperString::ConstASCIISequence::~ConstASCIISequence() {
    // nothing go here
}

SuperString::Size SuperString::ConstASCIISequence::length() const /*override*/ {
    if(this->_lengthComputed == SuperString::FALSE) {
        void *ptr;
        const char *pointer = this->_chars;
        while(*pointer != '\0') {
            pointer++;
        }
        // I need to keep this method const
        ptr = ((void *) ((unsigned long) &this->_lengthComputed));
        *((Bool *) ptr) = SuperString::TRUE;
        ptr = ((void *) ((unsigned long) &this->_length));
        *((Size *) ptr) = pointer - this->_chars;
    }
    return this->_length;
}

SuperString::Result<int, SuperString::Error> SuperString::ConstASCIISequence::codeUnitAt(
        SuperString::Size index) const {
    if(index < this->length()) {
        return Result<int, SuperString::Error>(*(this->_chars + index));
    }
    return Result<int, SuperString::Error>(Error::RangeError);
}

SuperString::Result<SuperString, SuperString::Error>
SuperString::ConstASCIISequence::substring(SuperString::Size startIndex,
                                           SuperString::Size endIndex) const {
    if(this->length() < startIndex || this->length() < endIndex) {
        return Result<SuperString, Error>(Error::RangeError);
    }
    SubstringSequence *sequence = new SubstringSequence(this, startIndex, endIndex);
    return Result<SuperString, Error>(SuperString(sequence));
}

void SuperString::ConstASCIISequence::print(std::ostream &stream) const {
    stream << this->_chars;
}

void SuperString::ConstASCIISequence::print(std::ostream &stream, SuperString::Size startIndex,
                                            SuperString::Size endIndex) const {
    stream.write(this->_chars + startIndex, endIndex - startIndex);
}

SuperString SuperString::ConstASCIISequence::trim() const {
    Size startIndex = 0;
    Size endIndex = this->length();
    char c = *(this->_chars + startIndex);
    while(c != '\0' && SuperString::isWhiteSpace(c)) {
        c = *(this->_chars + (++startIndex));
    }
    c = *(this->_chars + (endIndex - 1));
    while(endIndex > 0 && SuperString::isWhiteSpace(c)) {
        c = *(this->_chars + (--endIndex - 1));
    }
    return this->substring(startIndex, endIndex).ok(); // TODO:
}

SuperString SuperString::ConstASCIISequence::trimLeft() const {
    Size startIndex = 0;
    char c = *(this->_chars + startIndex);
    while(c != '\0' && SuperString::isWhiteSpace(c)) {
        c = *(this->_chars + ++startIndex);
    }
    return this->substring(startIndex, this->length()).ok(); // TODO:
}

SuperString SuperString::ConstASCIISequence::trimRight() const {
    Size endIndex = this->length();
    char c = *(this->_chars + (endIndex - 1));
    while(endIndex > 0 && SuperString::isWhiteSpace(c)) {
        c = *(this->_chars + (--endIndex - 1));
    }
    return this->substring(0, endIndex).ok(); // TODO:
}

SuperString::Size SuperString::ConstASCIISequence::keepingCost() const {
    return sizeof(ConstASCIISequence);
}

//*-- SuperString::CopyASCIISequence (internal)
SuperString::CopyASCIISequence::CopyASCIISequence(const char *chars) {
    const char *pointer = chars;
    while(*pointer != '\0') {
        pointer++;
    }
    this->_length = pointer - chars;
    this->_chars = new char[this->_length + 1];
    for(Size i = 0; i <= this->_length; i++) {
        *(this->_chars + i) = *(chars + i);
    }
}

SuperString::CopyASCIISequence::CopyASCIISequence(const SuperString::ConstASCIISequence *sequence) {
    this->_length = sequence->length();
    this->_chars = new char[this->_length + 1];
    for(Size i = 0; i <= this->_length; i++) {
        *(this->_chars + i) = *(sequence->_chars + i);
    }
}

SuperString::CopyASCIISequence::~CopyASCIISequence() {
    delete this->_chars;
}

SuperString::Size SuperString::CopyASCIISequence::length() const {
    return this->_length;
}

SuperString::Result<int, SuperString::Error> SuperString::CopyASCIISequence::codeUnitAt(
        SuperString::Size index) const {
    if(index < this->length()) {
        return Result<int, SuperString::Error>(*(this->_chars + index));
    }
    return Result<int, SuperString::Error>(Error::RangeError);
}

SuperString::Result<SuperString, SuperString::Error>
SuperString::CopyASCIISequence::substring(SuperString::Size startIndex,
                                          SuperString::Size endIndex) const {
    if(this->length() < startIndex || this->length() < endIndex) {
        return Result<SuperString, Error>(Error::RangeError);
    }
    SubstringSequence *sequence = new SubstringSequence(this, startIndex, endIndex);
    return Result<SuperString, Error>(SuperString(sequence));
}

void SuperString::CopyASCIISequence::print(std::ostream &stream) const {
    stream << this->_chars;
}

void SuperString::CopyASCIISequence::print(std::ostream &stream, SuperString::Size startIndex,
                                           SuperString::Size endIndex) const {
    stream.write(this->_chars + startIndex, endIndex - startIndex);
}

SuperString SuperString::CopyASCIISequence::trim() const {
    Size startIndex = 0;
    Size endIndex = this->length();
    char c = *(this->_chars + startIndex);
    while(c != '\0' && SuperString::isWhiteSpace(c)) {
        c = *(this->_chars + (++startIndex));
    }
    c = *(this->_chars + (endIndex - 1));
    while(endIndex > 0 && SuperString::isWhiteSpace(c)) {
        c = *(this->_chars + (--endIndex - 1));
    }
    return this->substring(startIndex, endIndex).ok(); // TODO:
}

SuperString SuperString::CopyASCIISequence::trimLeft() const {
    Size startIndex = 0;
    char c = *(this->_chars + startIndex);
    while(c != '\0' && SuperString::isWhiteSpace(c)) {
        c = *(this->_chars + ++startIndex);
    }
    return this->substring(startIndex, this->length()).ok(); // TODO:
}

SuperString SuperString::CopyASCIISequence::trimRight() const {
    Size endIndex = this->length();
    char c = *(this->_chars + (endIndex - 1));
    while(endIndex > 0 && SuperString::isWhiteSpace(c)) {
        c = *(this->_chars + (--endIndex - 1));
    }
    return this->substring(0, endIndex).ok(); // TODO:
}

SuperString::Size SuperString::CopyASCIISequence::keepingCost() const {
    Size cost = sizeof(CopyASCIISequence);
    if(this->_chars != NULL) {
        cost += this->length() + 1;
    }
    return cost;
}

//*-- SuperString::ConstUTF8Sequence (internal)
SuperString::ConstUTF8Sequence::ConstUTF8Sequence(const char *chars)
        : _chars(chars),
          _lengthComputed(SuperString::FALSE) {
    // nothing go here
}

SuperString::ConstUTF8Sequence::~ConstUTF8Sequence() {
    // nothing go here
}

SuperString::Size SuperString::ConstUTF8Sequence::length() const /*override*/ {
    if(this->_lengthComputed == SuperString::FALSE) {
        Size length = 0;
        const unsigned char *pointer = (unsigned char *) this->_chars;
        while(*pointer != '\0') {
            if((*pointer & 0xf8) == 0xf0) { pointer += 4; }
            else if((*pointer & 0xf0) == 0xe0) { pointer += 3; }
            else if((*pointer & 0xe0) == 0xc0) { pointer += 2; }
            else if((*pointer & 0x80) == 0x00) { pointer++; }
            else return 0;
            length++;
        }
        // I need to keep this method const
        void *ptr = ((void *) ((unsigned long) &this->_lengthComputed));
        *((Bool *) ptr) = SuperString::TRUE;
        ptr = ((void *) ((unsigned long) &this->_length));
        *((Size *) ptr) = length;
    }
    return this->_length;
}

SuperString::Result<int, SuperString::Error> SuperString::ConstUTF8Sequence::codeUnitAt(SuperString::Size index) const {
    if(index < this->length()) {
        Size i = 0;
        const unsigned char *pointer = (unsigned char *) this->_chars;
        while(*pointer != '\0') {
            int codeUnit = 0;
            int remainingBytes = 0;
            if((*pointer & 0xf8) == 0xf0) {
                codeUnit = *pointer & 0x07;
                remainingBytes = 3;
            } else if((*pointer & 0xf0) == 0xe0) {
                codeUnit = *pointer & 0x0f;
                remainingBytes = 2;
            } else if((*pointer & 0xe0) == 0xc0) {
                codeUnit = *pointer & 0x1f;
                remainingBytes = 1;
            } else if((*pointer & 0x80) == 0x00) {
                codeUnit = *pointer;
            } else {
                return Result<int, SuperString::Error>(Error::InvalidByteSequence);
            }
            while(remainingBytes-- > 0) {
                pointer++;
                //remainingBytes--;
                codeUnit = codeUnit << 6 | (*pointer & 0x3f);
            }
            if(i == index) {
                return Result<int, SuperString::Error>(codeUnit);
            }
            pointer++;
            i++;
        }
    }
    return Result<int, SuperString::Error>(Error::RangeError);
}

SuperString::Result<SuperString, SuperString::Error>
SuperString::ConstUTF8Sequence::substring(SuperString::Size startIndex,
                                          SuperString::Size endIndex) const {
    Size length = this->length();
    if(length < startIndex || length < endIndex) {
        return Result<SuperString, Error>(Error::RangeError);
    }
    SubstringSequence *sequence = new SubstringSequence(this, startIndex, endIndex);
    return Result<SuperString, Error>(SuperString(sequence));
}

void SuperString::ConstUTF8Sequence::print(std::ostream &stream) const {
    stream << this->_chars;
}

void SuperString::ConstUTF8Sequence::print(std::ostream &stream, SuperString::Size startIndex,
                                           SuperString::Size endIndex) const {
    Result<Pair<Size, Size>, Error> result = SuperString::_UTF8_offsetOfRange(this->_chars, startIndex, endIndex);
    if(result.isOk()) {
        stream.write(this->_chars + result.ok().first(), result.ok().second() - result.ok().first());
    }
}

SuperString SuperString::ConstUTF8Sequence::trim() const {
    // TODO: General code, specify
    Size startIndex = 0;
    Size endIndex = this->length();
    Result<int, Error> result = this->codeUnitAt(startIndex);
    while(result.isOk() && SuperString::isWhiteSpace(result.ok())) {
        result = this->codeUnitAt(++startIndex);
    }
    result = this->codeUnitAt(endIndex - 1);
    while(result.isOk() && SuperString::isWhiteSpace(result.ok())) {
        result = this->codeUnitAt(--endIndex - 1);
    }
    return this->substring(startIndex, endIndex).ok(); // TODO:
}

SuperString SuperString::ConstUTF8Sequence::trimLeft() const {
    // TODO: General code, specify
    Size startIndex = 0;
    Result<int, Error> result = this->codeUnitAt(startIndex);
    while(result.isOk() && SuperString::isWhiteSpace(result.ok())) {
        result = this->codeUnitAt(++startIndex);
    }
    return this->substring(startIndex, this->length()).ok(); // TODO:
}

SuperString SuperString::ConstUTF8Sequence::trimRight() const {
    // TODO: General code, specify
    Size endIndex = this->length();
    Result<int, Error> result = this->codeUnitAt(endIndex - 1);
    while(result.isOk() && SuperString::isWhiteSpace(result.ok())) {
        result = this->codeUnitAt(--endIndex - 1);
    }
    return this->substring(0, endIndex).ok(); // TODO:
}

SuperString::Size SuperString::ConstUTF8Sequence::keepingCost() const {
    return sizeof(ConstUTF8Sequence);
}

//*-- SuperString::CopyUTF8Sequence (internal)
SuperString::CopyUTF8Sequence::CopyUTF8Sequence(const char *chars)
        : _lengthComputed(FALSE),
          _memLength(0) {
    const char *pointer = chars;
    while(*pointer != '\0') {
        pointer++;
    }
    this->_memLength = pointer - chars + 1;
    this->_chars = new char[this->_memLength];
    for(Size i = 0; i < this->_memLength; i++) {
        *(this->_chars + i) = *(chars + i);
    }
}

SuperString::CopyUTF8Sequence::CopyUTF8Sequence(const SuperString::ConstUTF8Sequence *sequence)
        : _lengthComputed(FALSE),
          _memLength(0) {
    const char *pointer = sequence->_chars;
    while(*pointer != '\0') {
        pointer++;
    }
    this->_memLength = pointer - sequence->_chars + 1;
    this->_chars = new char[this->_memLength];
    for(Size i = 0; i < this->_memLength; i++) {
        *(this->_chars + i) = *(sequence->_chars + i);
    }
}

SuperString::CopyUTF8Sequence::~CopyUTF8Sequence() {
    delete this->_chars;
}

SuperString::Size SuperString::CopyUTF8Sequence::length() const {
    if(this->_lengthComputed == SuperString::FALSE) {
        Size length = 0;
        const unsigned char *pointer = (unsigned char *) this->_chars;
        while(*pointer != '\0') {
            if((*pointer & 0xf8) == 0xf0) { pointer += 4; }
            else if((*pointer & 0xf0) == 0xe0) { pointer += 3; }
            else if((*pointer & 0xe0) == 0xc0) { pointer += 2; }
            else if((*pointer & 0x80) == 0x00) { pointer++; }
            else return 0;
            length++;
        }
        // I need to keep this method const
        void *ptr = ((void *) ((unsigned long) &this->_lengthComputed));
        *((Bool *) ptr) = SuperString::TRUE;
        ptr = ((void *) ((unsigned long) &this->_length));
        *((Size *) ptr) = length;
    }
    return this->_length;
}

SuperString::Result<int, SuperString::Error> SuperString::CopyUTF8Sequence::codeUnitAt(SuperString::Size index) const {
    if(index < this->length()) {
        Size i = 0;
        const unsigned char *pointer = (unsigned char *) this->_chars;
        while(*pointer != '\0') {
            int codeUnit = 0;
            int remainingBytes = 0;
            if((*pointer & 0xf8) == 0xf0) {
                codeUnit = *pointer & 0x07;
                remainingBytes = 3;
            } else if((*pointer & 0xf0) == 0xe0) {
                codeUnit = *pointer & 0x0f;
                remainingBytes = 2;
            } else if((*pointer & 0xe0) == 0xc0) {
                codeUnit = *pointer & 0x1f;
                remainingBytes = 1;
            } else if((*pointer & 0x80) == 0x00) {
                codeUnit = *pointer;
            } else {
                return Result<int, SuperString::Error>(Error::InvalidByteSequence);
            }
            while(remainingBytes-- > 0) {
                pointer++;
                //remainingBytes--;
                codeUnit = codeUnit << 6 | (*pointer & 0x3f);
            }
            if(i == index) {
                return Result<int, SuperString::Error>(codeUnit);
            }
            pointer++;
            i++;
        }
    }
    return Result<int, SuperString::Error>(Error::RangeError);
}

SuperString::Result<SuperString, SuperString::Error>
SuperString::CopyUTF8Sequence::substring(SuperString::Size startIndex, SuperString::Size endIndex) const {
    if(this->length() < startIndex || this->length() < endIndex) {
        return Result<SuperString, Error>(Error::RangeError);
    }
    SubstringSequence *sequence = new SubstringSequence(this, startIndex, endIndex);
    return Result<SuperString, Error>(SuperString(sequence));
}

void SuperString::CopyUTF8Sequence::print(std::ostream &stream) const {
    stream << this->_chars;
}

void SuperString::CopyUTF8Sequence::print(std::ostream &stream, SuperString::Size startIndex,
                                          SuperString::Size endIndex) const {
    Result<Pair<Size, Size>, Error> result = SuperString::_UTF8_offsetOfRange(this->_chars, startIndex, endIndex);
    if(result.isOk()) {
        stream.write(this->_chars + result.ok().first(), result.ok().second() - result.ok().first());
    }
}

SuperString SuperString::CopyUTF8Sequence::trim() const {
    // TODO: General code, specify
    Size startIndex = 0;
    Size endIndex = this->length();
    Result<int, Error> result = this->codeUnitAt(startIndex);
    while(result.isOk() && SuperString::isWhiteSpace(result.ok())) {
        result = this->codeUnitAt(++startIndex);
    }
    result = this->codeUnitAt(endIndex - 1);
    while(result.isOk() && SuperString::isWhiteSpace(result.ok())) {
        result = this->codeUnitAt(--endIndex - 1);
    }
    return this->substring(startIndex, endIndex).ok(); // TODO:
}

SuperString SuperString::CopyUTF8Sequence::trimLeft() const {
    // TODO: General code, specify
    Size startIndex = 0;
    Result<int, Error> result = this->codeUnitAt(startIndex);
    while(result.isOk() && SuperString::isWhiteSpace(result.ok())) {
        result = this->codeUnitAt(++startIndex);
    }
    return this->substring(startIndex, this->length()).ok(); // TODO:
}

SuperString SuperString::CopyUTF8Sequence::trimRight() const {
    // TODO: General code, specify
    Size endIndex = this->length();
    Result<int, Error> result = this->codeUnitAt(endIndex - 1);
    while(result.isOk() && SuperString::isWhiteSpace(result.ok())) {
        result = this->codeUnitAt(--endIndex - 1);
    }
    return this->substring(0, endIndex).ok(); // TODO:
}

SuperString::Size SuperString::CopyUTF8Sequence::keepingCost() const {
    Size cost = sizeof(CopyUTF8Sequence) + this->_memLength;
    return cost;
}

// SuperString::_UTF8_offsetOfRange
SuperString::Result<SuperString::Pair<SuperString::Size, SuperString::Size>, SuperString::Error>
SuperString::_UTF8_offsetOfRange(const char *chars, Size startIndex, Size endIndex) {
    Size i = 0;
    Size startOffset, endOffset;
    Bool first = FALSE, second = FALSE;
    const unsigned char *pointer = (unsigned char *) chars;
    while(*pointer != '\0') {
        if(!first) {
            if(i == startIndex) {
                first = TRUE;
                startOffset = ((Size) pointer) - ((Size) chars);
            }
        } else {
            if(i == endIndex) {
                second = TRUE;
                endOffset = ((Size) pointer) - ((Size) chars);
                break;
            }
        }
        if((*pointer & 0xf8) == 0xf0) { pointer += 4; }
        else if((*pointer & 0xf0) == 0xe0) { pointer += 3; }
        else if((*pointer & 0xe0) == 0xc0) { pointer += 2; }
        else if((*pointer & 0x80) == 0x00) { pointer++; }
        else return Result<Pair<Size, Size>, Error>(Error::InvalidByteSequence);
        i++;
    }
    if(!second) {
        if(i == endIndex) {
            second = TRUE;
            endOffset = ((Size) pointer) - ((Size) chars);
        }
    }
    if(first && second) {
        return Result<Pair<Size, Size>, Error>(Pair<Size, Size>(startOffset, endOffset));
    }
    return Result<Pair<Size, Size>, Error>(Error::RangeError);
}

//*-- ConstUTF16Sequence (internal)
SuperString::ConstUTF16Sequence::ConstUTF16Sequence(const SuperString::Byte *chars)
        : _chars(chars),
          _lengthComputed(FALSE) {
    // nothing go here
}

SuperString::ConstUTF16Sequence::~ConstUTF16Sequence() {
    // nothing go here
}

SuperString::Size SuperString::ConstUTF16Sequence::length() const /*override*/ {
    if(this->_lengthComputed == SuperString::FALSE) {
        void *ptr;
        const Byte *pointer = this->_chars;
        while(*pointer != 0x00 || *(pointer + 1) != 0x00) {
            pointer += 2;
        }
        // I need to keep this method const
        ptr = ((void *) ((unsigned long) &this->_lengthComputed));
        *((Bool *) ptr) = SuperString::TRUE;
        ptr = ((void *) ((unsigned long) &this->_length));
        *((Size *) ptr) = (pointer - this->_chars) / 2;
    }
    return this->_length;
}

SuperString::Result<int, SuperString::Error> SuperString::ConstUTF16Sequence::codeUnitAt(
        SuperString::Size index) const {
    if(index < this->length()) {
        return Result<int, SuperString::Error>((*(this->_chars + index * 2) << 8) | *(this->_chars + index * 2 + 1));
    }
    return Result<int, SuperString::Error>(Error::RangeError);
}

SuperString::Result<SuperString, SuperString::Error>
SuperString::ConstUTF16Sequence::substring(SuperString::Size startIndex,
                                           SuperString::Size endIndex) const {
    if(this->length() < startIndex || this->length() < endIndex) {
        return Result<SuperString, Error>(Error::RangeError);
    }
    SubstringSequence *sequence = new SubstringSequence(this, startIndex, endIndex);
    return Result<SuperString, Error>(SuperString(sequence));
}

void SuperString::ConstUTF16Sequence::print(std::ostream &stream) const {
    this->print(stream, 0, this->length());
}

void SuperString::ConstUTF16Sequence::print(std::ostream &stream, SuperString::Size startIndex,
                                            SuperString::Size endIndex) const {
    for(Size i = startIndex, length = this->length(); i < length && i < endIndex; i++) {
        int c = this->codeUnitAt(i).ok();
        char numBytes = 0;
        Byte byte1, byte2, byte3, byte4;
        if(c < 0x0080) {
            byte1 = (Byte) c;
            numBytes = 1;
        } else if(c < 0x0800) {
            byte1 = (Byte) ((c >> 6) + (((Byte) 0b110) << 5));
            byte2 = (Byte) ((((Byte) 0x2) << 6) + (c & 0b00000111111));
            numBytes = 2;
        } else if(c < 0x10000) {
            byte1 = (Byte) ((c >> 12) + (((Byte) 0b1110) << 4));
            byte2 = (Byte) ((((Byte) 0x2) << 6) + (c >> 6 & 0b0000111111));
            byte3 = (Byte) ((((Byte) 0x2) << 6) + (c & 0b0000000000111111));
            numBytes = 3;
        } else if(c < 0x200000) {
            byte1 = (Byte) ((c >> 18) + (((Byte) 0b11110) << 3));
            byte2 = (Byte) ((((Byte) 0x2) << 6) + (c >> 12 & 0b000111111));
            byte3 = (Byte) ((((Byte) 0x2) << 6) + (c >> 6 & 0b000000000111111));
            byte4 = (Byte) ((((Byte) 0x2) << 6) + (c & 0b00000000000000111111));
            numBytes = 4;
        }
        char *printable = new char[numBytes];
        Size j = 0;
        while(j < numBytes) {
            switch(j) {
                case 0:
                    printable[j] = byte1;
                    break;
                case 1:
                    printable[j] = byte2;
                    break;
                case 2:
                    printable[j] = byte3;
                    break;
                case 3:
                    printable[j] = byte4;
                    break;
                default:
                    ;
            }
            j++;
        }
        stream.write(printable, numBytes);
        delete[] printable;
    }
}

SuperString SuperString::ConstUTF16Sequence::trim() const {
    // TODO: General code, specify
    Size startIndex = 0;
    Size endIndex = this->length();
    Result<int, Error> result = this->codeUnitAt(startIndex);
    while(result.isOk() && SuperString::isWhiteSpace(result.ok())) {
        result = this->codeUnitAt(++startIndex);
    }
    result = this->codeUnitAt(endIndex - 1);
    while(result.isOk() && SuperString::isWhiteSpace(result.ok())) {
        result = this->codeUnitAt(--endIndex - 1);
    }
    return this->substring(startIndex, endIndex).ok(); // TODO:
}

SuperString SuperString::ConstUTF16Sequence::trimLeft() const {
    // TODO: General code, specify
    Size startIndex = 0;
    Result<int, Error> result = this->codeUnitAt(startIndex);
    while(result.isOk() && SuperString::isWhiteSpace(result.ok())) {
        result = this->codeUnitAt(++startIndex);
    }
    return this->substring(startIndex, this->length()).ok(); // TODO:
}

SuperString SuperString::ConstUTF16Sequence::trimRight() const {
    // TODO: General code, specify
    Size endIndex = this->length();
    Result<int, Error> result = this->codeUnitAt(endIndex - 1);
    while(result.isOk() && SuperString::isWhiteSpace(result.ok())) {
        result = this->codeUnitAt(--endIndex - 1);
    }
    return this->substring(0, endIndex).ok(); // TODO:
}

SuperString::Size SuperString::ConstUTF16Sequence::keepingCost() const {
    return sizeof(ConstUTF16Sequence);
}

//*-- SuperString::CopyUTF16Sequence (internal)
SuperString::CopyUTF16Sequence::CopyUTF16Sequence(const SuperString::Byte *chars) {
    Size memLength;
    const Byte *pointer = chars;
    while(*pointer != 0x00 || *(pointer + 1) != 0x00) {
        pointer += 2;
    }
    memLength = pointer - chars + 2;
    this->_length = (pointer - chars) / 2;
    this->_chars = new Byte[memLength];
    for(Size i = 0; i < memLength; i++) {
        *(this->_chars + i) = *(chars + i);
    }
}

SuperString::CopyUTF16Sequence::CopyUTF16Sequence(const SuperString::ConstUTF16Sequence *sequence) {
    Size memLength;
    const Byte *pointer = sequence->_chars;
    while(*pointer != 0x00 || *(pointer + 1) != 0x00) {
        pointer++;
    }
    memLength = pointer - sequence->_chars + 2;
    this->_length = (pointer - sequence->_chars) / 2;
    this->_chars = new Byte[memLength];
    for(Size i = 0; i < memLength; i++) {
        *(this->_chars + i) = *(sequence->_chars + i);
    }
}

SuperString::CopyUTF16Sequence::~CopyUTF16Sequence() {
    delete this->_chars;
}

SuperString::Size SuperString::CopyUTF16Sequence::length() const {
    return this->_length;
}

SuperString::Result<int, SuperString::Error> SuperString::CopyUTF16Sequence::codeUnitAt(SuperString::Size index) const {
    if(index < this->length()) {
        return Result<int, SuperString::Error>((*(this->_chars + index * 2) << 8) | *(this->_chars + index * 2 + 1));
    }
    return Result<int, SuperString::Error>(Error::RangeError);
}

SuperString::Result<SuperString, SuperString::Error>
SuperString::CopyUTF16Sequence::substring(SuperString::Size startIndex, SuperString::Size endIndex) const {
    if(this->length() < startIndex || this->length() < endIndex) {
        return Result<SuperString, Error>(Error::RangeError);
    }
    SubstringSequence *sequence = new SubstringSequence(this, startIndex, endIndex);
    return Result<SuperString, Error>(SuperString(sequence));
}

void SuperString::CopyUTF16Sequence::print(std::ostream &stream) const {
    this->print(stream, 0, this->length());
}

void SuperString::CopyUTF16Sequence::print(std::ostream &stream, SuperString::Size startIndex,
                                          SuperString::Size endIndex) const {
    for(Size i = startIndex, length = this->length(); i < length && i < endIndex; i++) {
        int c = this->codeUnitAt(i).ok();
        char numBytes = 0;
        Byte byte1, byte2, byte3, byte4;
        if(c < 0x0080) {
            byte1 = (Byte) c;
            numBytes = 1;
        } else if(c < 0x0800) {
            byte1 = (Byte) ((c >> 6) + (((Byte) 0b110) << 5));
            byte2 = (Byte) ((((Byte) 0x2) << 6) + (c & 0b00000111111));
            numBytes = 2;
        } else if(c < 0x10000) {
            byte1 = (Byte) ((c >> 12) + (((Byte) 0b1110) << 4));
            byte2 = (Byte) ((((Byte) 0x2) << 6) + (c >> 6 & 0b0000111111));
            byte3 = (Byte) ((((Byte) 0x2) << 6) + (c & 0b0000000000111111));
            numBytes = 3;
        } else if(c < 0x200000) {
            byte1 = (Byte) ((c >> 18) + (((Byte) 0b11110) << 3));
            byte2 = (Byte) ((((Byte) 0x2) << 6) + (c >> 12 & 0b000111111));
            byte3 = (Byte) ((((Byte) 0x2) << 6) + (c >> 6 & 0b000000000111111));
            byte4 = (Byte) ((((Byte) 0x2) << 6) + (c & 0b00000000000000111111));
            numBytes = 4;
        }
        char *printable = new char[numBytes];
        Size j = 0;
        while(j < numBytes) {
            switch(j) {
                case 0:
                    printable[j] = byte1;
                    break;
                case 1:
                    printable[j] = byte2;
                    break;
                case 2:
                    printable[j] = byte3;
                    break;
                case 3:
                    printable[j] = byte4;
                    break;
                default:
                    ;
            }
            j++;
        }
        stream.write(printable, numBytes);
        delete[] printable;
    }
}

SuperString SuperString::CopyUTF16Sequence::trim() const {
    // TODO: General code, specify
    Size startIndex = 0;
    Size endIndex = this->length();
    Result<int, Error> result = this->codeUnitAt(startIndex);
    while(result.isOk() && SuperString::isWhiteSpace(result.ok())) {
        result = this->codeUnitAt(++startIndex);
    }
    result = this->codeUnitAt(endIndex - 1);
    while(result.isOk() && SuperString::isWhiteSpace(result.ok())) {
        result = this->codeUnitAt(--endIndex - 1);
    }
    return this->substring(startIndex, endIndex).ok(); // TODO:
}

SuperString SuperString::CopyUTF16Sequence::trimLeft() const {
    // TODO: General code, specify
    Size startIndex = 0;
    Result<int, Error> result = this->codeUnitAt(startIndex);
    while(result.isOk() && SuperString::isWhiteSpace(result.ok())) {
        result = this->codeUnitAt(++startIndex);
    }
    return this->substring(startIndex, this->length()).ok(); // TODO:
}

SuperString SuperString::CopyUTF16Sequence::trimRight() const {
    // TODO: General code, specify
    Size endIndex = this->length();
    Result<int, Error> result = this->codeUnitAt(endIndex - 1);
    while(result.isOk() && SuperString::isWhiteSpace(result.ok())) {
        result = this->codeUnitAt(--endIndex - 1);
    }
    return this->substring(0, endIndex).ok(); // TODO:
}

SuperString::Size SuperString::CopyUTF16Sequence::keepingCost() const {
    Size cost = sizeof(CopyUTF8Sequence) + this->length() * 2 + 2;
    return cost;
}

//*-- SuperString::SubstringSequence (internal)
SuperString::SubstringSequence::SubstringSequence(const StringSequence *sequence, SuperString::Size startIndex,
                                                  SuperString::Size endIndex) {
    this->_kind = Kind::SUBSTRING;
    this->_container._substring._sequence = sequence;
    this->_container._substring._startIndex = startIndex;
    this->_container._substring._endIndex = endIndex;
    this->_container._substring._sequence->addReferencer(this);
}

SuperString::SubstringSequence::~SubstringSequence() {
    if(this->_kind == Kind::SUBSTRING) {
        this->_container._substring._sequence->removeReferencer(this);
    }
    // TODO:
}

SuperString::Size SuperString::SubstringSequence::length() const /*override*/ {
    if(this->_kind == Kind::SUBSTRING) {
        return this->_container._substring._endIndex - this->_container._substring._startIndex;
    }
    // else: this->_kind == Kind::CONTENTED
    return this->_container._contented._length;
}

SuperString::Result<int, SuperString::Error> SuperString::SubstringSequence::codeUnitAt(
        SuperString::Size index) const {
    if(this->_kind == Kind::SUBSTRING) {
        return this->_container._substring._sequence->codeUnitAt(this->_container._substring._startIndex + index);
    }
    // TODO:
    return Result<int, SuperString::Error>(Error::Unimplemented);
}

SuperString::Result<SuperString, SuperString::Error>
SuperString::SubstringSequence::substring(SuperString::Size startIndex, SuperString::Size endIndex) const {
    if(this->_kind == Kind::SUBSTRING) {
        if((this->_container._substring._sequence->length() < (this->_container._substring._startIndex + startIndex)) ||
           (this->_container._substring._sequence->length() < (this->_container._substring._startIndex + endIndex))) {
            return Result<SuperString, Error>(Error::RangeError);
        } else {
            return Result<SuperString, Error>(SuperString(new SubstringSequence(this->_container._substring._sequence,
                                                                                this->_container._substring._startIndex +
                                                                                startIndex,
                                                                                this->_container._substring._startIndex +
                                                                                endIndex)));
        }
    }
    // TODO:
    return Result<SuperString, Error>(Error::Unimplemented);
}

void SuperString::SubstringSequence::print(std::ostream &stream) const {
    this->_container._substring._sequence->print(stream, this->_container._substring._startIndex,
                                                 this->_container._substring._endIndex);
}

void SuperString::SubstringSequence::print(std::ostream &stream, SuperString::Size startIndex,
                                           SuperString::Size endIndex) const {
    if(this->_kind == Kind::SUBSTRING) {
        this->_container._substring._sequence->print(stream, this->_container._substring._startIndex + startIndex,
                                                     this->_container._substring._startIndex + endIndex);
    }
    // TODO:
}

SuperString SuperString::SubstringSequence::trim() const {
    // TODO: General code, specify
    Size startIndex = 0;
    Size endIndex = this->length();
    Result<int, Error> result = this->codeUnitAt(startIndex);
    while(result.isOk() && SuperString::isWhiteSpace(result.ok())) {
        result = this->codeUnitAt(++startIndex);
    }
    result = this->codeUnitAt(endIndex - 1);
    while(result.isOk() && SuperString::isWhiteSpace(result.ok())) {
        result = this->codeUnitAt(--endIndex - 1);
    }
    return this->substring(startIndex, endIndex).ok(); // TODO:
}

SuperString SuperString::SubstringSequence::trimLeft() const {
    // TODO: General code, specify
    Size startIndex = 0;
    Result<int, Error> result = this->codeUnitAt(startIndex);
    while(result.isOk() && SuperString::isWhiteSpace(result.ok())) {
        result = this->codeUnitAt(++startIndex);
    }
    return this->substring(startIndex, this->length()).ok(); // TODO:
}

SuperString SuperString::SubstringSequence::trimRight() const {
    // TODO: General code, specify
    Size endIndex = this->length();
    Result<int, Error> result = this->codeUnitAt(endIndex - 1);
    while(result.isOk() && SuperString::isWhiteSpace(result.ok())) {
        result = this->codeUnitAt(--endIndex - 1);
    }
    return this->substring(0, endIndex).ok(); // TODO:
}

SuperString::Size SuperString::SubstringSequence::keepingCost() const {
    if(this->_kind == Kind::SUBSTRING) {
        return sizeof(SubstringSequence) + this->_container._substring._sequence->keepingCost();
    }
    return 0;
}

SuperString::Size SuperString::SubstringSequence::reconstructionCost() const {
    if(this->_kind == Kind::SUBSTRING) {
        return sizeof(SubstringSequence) +
               (this->_container._substring._endIndex - this->_container._substring._startIndex) * 2;
    }
    return 0;
}

void SuperString::SubstringSequence::reconstruct(const StringSequence *sequence) const {

}

//*-- SuperString::ConcatenationSequence (internal)
SuperString::ConcatenationSequence::ConcatenationSequence(const StringSequence *leftSequence,
                                                          const StringSequence *rightSequence) {
    this->_kind = Kind::CONCATENATION;
    this->_container._concatenation._left = leftSequence;
    this->_container._concatenation._right = rightSequence;
    this->_container._concatenation._left->addReferencer(this);
    this->_container._concatenation._right->addReferencer(this);
}

SuperString::ConcatenationSequence::~ConcatenationSequence() {
    if(this->_kind == Kind::CONCATENATION) {
        this->_container._concatenation._left->removeReferencer(this);
        this->_container._concatenation._right->removeReferencer(this);
    }
}

SuperString::Size SuperString::ConcatenationSequence::length() const {
    if(this->_kind == Kind::CONCATENATION) {
        return this->_container._concatenation._left->length() + this->_container._concatenation._right->length();
    }
    // TODO:
    return 0;
}

SuperString::Result<int, SuperString::Error> SuperString::ConcatenationSequence::codeUnitAt(
        SuperString::Size index) const {
    if(this->_kind == Kind::CONCATENATION) {
        if(index < this->_container._concatenation._left->length()) {
            return this->_container._concatenation._left->codeUnitAt(index);
        } else if((index - this->_container._concatenation._left->length()) <
                  this->_container._concatenation._right->length()) {
            return this->_container._concatenation._right->codeUnitAt(
                    index - this->_container._concatenation._left->length());
        } else {
            return Result<int, Error>(Error::RangeError);
        }
    }
    // TODO:
    return Result<int, Error>(Error::Unimplemented);
}

SuperString::Result<SuperString, SuperString::Error>
SuperString::ConcatenationSequence::substring(SuperString::Size startIndex,
                                              SuperString::Size endIndex) const {
    if(this->length() < startIndex || this->length() < endIndex) {
        return Result<SuperString, Error>(Error::RangeError);
    }
    SubstringSequence *sequence = new SubstringSequence(this, startIndex, endIndex);
    return Result<SuperString, Error>(SuperString(sequence));
}

void SuperString::ConcatenationSequence::print(std::ostream &stream) const {
    if(this->_kind == Kind::CONCATENATION) {
        this->_container._concatenation._left->print(stream);
        this->_container._concatenation._right->print(stream);
    }
}

void SuperString::ConcatenationSequence::print(std::ostream &stream, SuperString::Size startIndex,
                                               SuperString::Size endIndex) const {
    if(this->_kind == Kind::CONCATENATION) {
        if(startIndex < this->_container._concatenation._left->length()) {
            if(endIndex < this->_container._concatenation._left->length()) {
                this->_container._concatenation._left->print(stream, startIndex, endIndex);
            } else {
                this->_container._concatenation._left->print(stream, startIndex,
                                                             this->_container._concatenation._left->length());
                this->_container._concatenation._right->print(stream, 0,
                                                              endIndex -
                                                              this->_container._concatenation._left->length());
            }
        } else {
            if((endIndex - this->_container._concatenation._left->length()) <
               this->_container._concatenation._right->length()) {
                this->_container._concatenation._right->print(stream, startIndex -
                                                                      this->_container._concatenation._left->length(),
                                                              endIndex -
                                                              this->_container._concatenation._left->length());
            }
        }
    }
    // TODO:
}

SuperString SuperString::ConcatenationSequence::trim() const {
    // TODO: General code, specify
    Size startIndex = 0;
    Size endIndex = this->length();
    Result<int, Error> result = this->codeUnitAt(startIndex);
    while(result.isOk() && SuperString::isWhiteSpace(result.ok())) {
        result = this->codeUnitAt(++startIndex);
    }
    result = this->codeUnitAt(endIndex - 1);
    while(result.isOk() && SuperString::isWhiteSpace(result.ok())) {
        result = this->codeUnitAt(--endIndex - 1);
    }
    return this->substring(startIndex, endIndex).ok(); // TODO:
}

SuperString SuperString::ConcatenationSequence::trimLeft() const {
    // TODO: General code, specify
    Size startIndex = 0;
    Result<int, Error> result = this->codeUnitAt(startIndex);
    while(result.isOk() && SuperString::isWhiteSpace(result.ok())) {
        result = this->codeUnitAt(++startIndex);
    }
    return this->substring(startIndex, this->length()).ok(); // TODO:
}

SuperString SuperString::ConcatenationSequence::trimRight() const {
    // TODO: General code, specify
    Size endIndex = this->length();
    Result<int, Error> result = this->codeUnitAt(endIndex - 1);
    while(result.isOk() && SuperString::isWhiteSpace(result.ok())) {
        result = this->codeUnitAt(--endIndex - 1);
    }
    return this->substring(0, endIndex).ok(); // TODO:
}

SuperString::Size SuperString::ConcatenationSequence::keepingCost() const {
    if(this->_kind == Kind::CONCATENATION) {
        return sizeof(SubstringSequence) + this->_container._concatenation._left->keepingCost() +
               this->_container._concatenation._right->keepingCost();
    }
    return 0;
}

SuperString::Size SuperString::ConcatenationSequence::reconstructionCost() const {
    if(this->_kind == Kind::CONCATENATION) {
        return sizeof(SubstringSequence) +
               (this->_container._concatenation._left->length() + this->_container._concatenation._right->length()) * 2;
    }
    return 0;
}

void SuperString::ConcatenationSequence::reconstruct(const StringSequence *sequence) const {

}

//*-- MultipleSequence (internal)
SuperString::MultipleSequence::MultipleSequence(const StringSequence *sequence, Size time) {
    this->_kind = Kind::MULTIPLE;
    this->_container._multiple._time = time;
    this->_container._multiple._sequence = sequence;
    this->_container._multiple._sequence->addReferencer(this);
}

SuperString::MultipleSequence::~MultipleSequence() {
    if(this->_kind == Kind::MULTIPLE) {
        this->_container._multiple._sequence->removeReferencer(this);
    }
}

SuperString::Size SuperString::MultipleSequence::length() const {
    if(this->_kind == Kind::MULTIPLE) {
        return this->_container._multiple._sequence->length() * this->_container._multiple._time;
    }
    // TODO:
    return 0;
}

SuperString::Result<int, SuperString::Error> SuperString::MultipleSequence::codeUnitAt(SuperString::Size index) const {
    if(this->_kind == Kind::MULTIPLE) {
        Size length = this->length();
        if(index < length) {
            return this->_container._multiple._sequence->codeUnitAt(
                    index % this->_container._multiple._sequence->length());
        }
        return Result<int, Error>(Error::RangeError);
    }
    // TODO:
    return Result<int, Error>(Error::Unimplemented);
}

SuperString::Result<SuperString, SuperString::Error>
SuperString::MultipleSequence::substring(SuperString::Size startIndex,
                                         SuperString::Size endIndex) const {
    Size length = this->length();
    if(length < startIndex || length < endIndex) {
        return Result<SuperString, Error>(Error::RangeError);
    }
    SubstringSequence *sequence = new SubstringSequence(this, startIndex, endIndex);
    return Result<SuperString, Error>(SuperString(sequence));
}

void SuperString::MultipleSequence::print(std::ostream &stream) const {
    if(this->_kind == Kind::MULTIPLE) {
        for(Size i = 0; i < this->_container._multiple._time; i++) {
            this->_container._multiple._sequence->print(stream);
        }
    }
}

void SuperString::MultipleSequence::print(std::ostream &stream, SuperString::Size startIndex,
                                          SuperString::Size endIndex) const {
    if(this->_kind == Kind::MULTIPLE) {
        Bool printing = FALSE;
        Size unitLength = this->_container._multiple._sequence->length();
        for(Size i = 0; i < this->_container._multiple._time; i++) {
            Size iterationStartIndex = i * unitLength;
            Size iterationEndIndex = (i + 1) * unitLength;
            if(printing == FALSE) {
                if(iterationStartIndex <= startIndex) {
                    if(endIndex < iterationEndIndex) {
                        this->_container._multiple._sequence->print(stream, startIndex - iterationStartIndex,
                                                                    endIndex - iterationStartIndex);
                        break;
                    } else {
                        printing = TRUE;
                        this->_container._multiple._sequence->print(stream, startIndex - iterationStartIndex,
                                                                    unitLength);
                    }
                }
            } else {
                if(endIndex <= iterationEndIndex) {
                    this->_container._multiple._sequence->print(stream, 0, endIndex - iterationStartIndex);
                }
            }
        }
    }
    // TODO:
}

SuperString SuperString::MultipleSequence::trim() const {
    // TODO: General code, specify
    Size startIndex = 0;
    Size endIndex = this->length();
    Result<int, Error> result = this->codeUnitAt(startIndex);
    while(result.isOk() && SuperString::isWhiteSpace(result.ok())) {
        result = this->codeUnitAt(++startIndex);
    }
    result = this->codeUnitAt(endIndex - 1);
    while(result.isOk() && SuperString::isWhiteSpace(result.ok())) {
        result = this->codeUnitAt(--endIndex - 1);
    }
    return this->substring(startIndex, endIndex).ok(); // TODO:
}

SuperString SuperString::MultipleSequence::trimLeft() const {
    // TODO: General code, specify
    Size startIndex = 0;
    Result<int, Error> result = this->codeUnitAt(startIndex);
    while(result.isOk() && SuperString::isWhiteSpace(result.ok())) {
        result = this->codeUnitAt(++startIndex);
    }
    return this->substring(startIndex, this->length()).ok(); // TODO:
}

SuperString SuperString::MultipleSequence::trimRight() const {
    // TODO: General code, specify
    Size endIndex = this->length();
    Result<int, Error> result = this->codeUnitAt(endIndex - 1);
    while(result.isOk() && SuperString::isWhiteSpace(result.ok())) {
        result = this->codeUnitAt(--endIndex - 1);
    }
    return this->substring(0, endIndex).ok(); // TODO:
}

SuperString::Size SuperString::MultipleSequence::keepingCost() const {
    if(this->_kind == Kind::MULTIPLE) {
        return sizeof(SubstringSequence) + this->_container._multiple._sequence->keepingCost();
    }
    return 0;
}

SuperString::Size SuperString::MultipleSequence::reconstructionCost() const {
    if(this->_kind == Kind::MULTIPLE) {
        return sizeof(SubstringSequence) +
               this->_container._multiple._sequence->length() * this->_container._multiple._time * 2;
    }
    return 0;
}

void SuperString::MultipleSequence::reconstruct(const StringSequence *sequence) const {

}

std::ostream &operator<<(std::ostream &stream, const SuperString &string) {
    string.print(stream);
    return stream;
}

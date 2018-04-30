/*-- imports --*/

#include <SuperString.hh>
// std
#include <algorithm>
#include <cstddef>
#include <iostream>
#include <stdexcept>

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
        this->_sequence->doDelete();
    }
}

bool SuperString::isEmpty() const {
    return this->length() == 0;
}

bool SuperString::isNotEmpty() const {
    return this->length() != 0;
}

std::size_t SuperString::length() const {
    if(this->_sequence != NULL) {
        return this->_sequence->length();
    }
    return 0;
}

int SuperString::compareTo(const SuperString &other) const {
    std::size_t thisLength = this->length();
    std::size_t otherLength = other.length();
    std::size_t len = (thisLength < otherLength) ? thisLength : otherLength;
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

SuperString::Result<std::size_t, SuperString::Error> SuperString::indexOf(SuperString other) const {
    if(this->_sequence != NULL) {
        return this->_sequence->indexOf(other);
    }
    return Result<std::size_t, Error>(Error::NotFound);
}

SuperString::Result<std::size_t, SuperString::Error> SuperString::lastIndexOf(SuperString other) const {
    if(this->_sequence != NULL) {
        return this->_sequence->lastIndexOf(other);
    }
    return Result<std::size_t, Error>(Error::NotFound);
}

SuperString::Result<int, SuperString::Error> SuperString::codeUnitAt(std::size_t index) const {
    if(this->_sequence != NULL) {
        return this->_sequence->codeUnitAt(index);
    }
    return Result<int, SuperString::Error>(Error::Unexpected);
}

SuperString::Result<SuperString, SuperString::Error>
SuperString::substring(std::size_t startIndex, std::size_t endIndex) const {
    if(this->_sequence != NULL) {
        return this->_sequence->substring(startIndex, endIndex);
    }
    return Result<SuperString, Error>(Error::RangeError);
}

SuperString SuperString::substr(std::size_t pos, std::size_t len) const {
    SuperString::Result<SuperString, SuperString::Error> result = this->substring(pos, pos + len);
    if(result.isErr()) {
        throw std::out_of_range("");
    }
    return result.ok();
}

SuperString SuperString::substr(std::size_t pos) const {
    SuperString::Result<SuperString, SuperString::Error> result = this->substring(pos, this->length() - pos);
    if(result.isErr()) {
        throw std::out_of_range("");
    }
    return result.ok();
}

bool SuperString::print(std::ostream &stream) const {
    if(this->_sequence != NULL) {
        return this->_sequence->print(stream);
    }
    return true;
}

bool SuperString::print(std::ostream &stream, std::size_t startIndex, std::size_t endIndex) const {
    if(this->_sequence != NULL) {
        return this->_sequence->print(stream, startIndex, endIndex);
    }
    return true;
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
std::size_t SuperString::freeingCost() const {
    return this->_sequence->freeingCost();
}

std::size_t SuperString::keepingCost() const {
    return this->_sequence->keepingCost();
}

SuperString SuperString::operator+(const SuperString &other) const {
    ConcatenationSequence *sequence = new ConcatenationSequence(this->_sequence, other._sequence);
    return SuperString(sequence);
}

SuperString SuperString::operator*(std::size_t times) const {
    MultipleSequence *sequence = new MultipleSequence(this->_sequence, times);
    return SuperString(sequence);
}

SuperString &SuperString::operator=(const SuperString &other) {
    if(this != &other) {
        if(this->_sequence->refRelease() == 0) {
            this->_sequence->doDelete();
        }
        this->_sequence = other._sequence;
        this->_sequence->refAdd();
    }
    return *this;
}

bool SuperString::operator==(const SuperString &other) const {
    return this->compareTo(other) == 0;
}

SuperString SuperString::Const(const char *chars, SuperString::Encoding encoding) {
    StringSequence *sequence = NULL;
    switch(encoding) {
        case Encoding::ASCII:
            sequence = new SuperString::ConstASCIISequence((Byte *) chars);
            break;
        case Encoding::UTF8:
            sequence = new SuperString::ConstUTF8Sequence((Byte *) chars);
            break;
        case Encoding::UTF16BE:
            sequence = new SuperString::ConstUTF16BESequence((Byte *) chars);
            break;
        case Encoding::UTF32:
            sequence = new SuperString::ConstUTF32Sequence((Byte *) chars);
            break;
    }
    return SuperString(sequence);
}

SuperString SuperString::Const(const int *bytes, SuperString::Encoding encoding) {
    return SuperString::Const((const char *) bytes, encoding);
}

SuperString SuperString::Const(const SuperString::Byte *bytes, SuperString::Encoding encoding) {
    return SuperString::Const((const char *) bytes, encoding);
}

SuperString SuperString::Copy(const char *chars, Encoding encoding) {
    StringSequence *sequence = NULL;
    switch(encoding) {
        case Encoding::ASCII:
            sequence = new SuperString::CopyASCIISequence((Byte *) chars);
            break;
        case Encoding::UTF8:
            sequence = new SuperString::CopyUTF8Sequence((Byte *) chars);
            break;
        case Encoding::UTF16BE:
            sequence = new SuperString::CopyUTF16BESequence((Byte *) chars);
            break;
        case Encoding::UTF32:
            sequence = new SuperString::CopyUTF32Sequence((Byte *) chars);
            break;
    }
    return SuperString(sequence);
}

SuperString SuperString::Copy(const int *bytes, SuperString::Encoding encoding) {
    return SuperString::Copy((const char *) bytes, encoding);
}

SuperString SuperString::Copy(const SuperString::Byte *bytes, Encoding encoding) {
    return SuperString::Copy((const char *) bytes, encoding);
}

//*-- SuperString::StringSequence (abstract|internal)
SuperString::StringSequence::StringSequence()
        : _refCount(0) {
    // nothing go here
}

SuperString::StringSequence::~StringSequence() {
    // nothing go here
}

bool SuperString::StringSequence::isEmpty() const {
    return this->length() == 0;
}

bool SuperString::StringSequence::isNotEmpty() const {
    return this->length() > 0;
}

SuperString::Result<std::size_t, SuperString::Error> SuperString::StringSequence::indexOf(SuperString other) const {
    for(std::size_t i = 0, length = this->length(); i < (length - other.length()); i++) {
        if(this->_substringMatches(i, other)) {
            return Result<std::size_t, Error>(i);
        }
    }
    return Result<std::size_t, Error>(Error::NotFound);
}

SuperString::Result<std::size_t, SuperString::Error> SuperString::StringSequence::lastIndexOf(SuperString other) const {
    for(std::size_t i = this->length() - other.length(); i > 0; i--) {
        if(this->_substringMatches(i - 1, other)) {
            return Result<std::size_t, Error>(i - 1);
        }
    }
    return Result<std::size_t, Error>(Error::NotFound);
}

void SuperString::StringSequence::refAdd() const {
    StringSequence *self = (StringSequence *) (unsigned long) this;
    self->_refCount++;
}

std::size_t SuperString::StringSequence::refRelease() const {
    StringSequence *self = (StringSequence *) (unsigned long) this;
    if(self->_refCount == 0) {
        return 0;
    }
    return --self->_refCount;
}

std::size_t SuperString::StringSequence::refCount() const {
    return this->_refCount;
}

void SuperString::StringSequence::addReferencer(SuperString::ReferenceStringSequence *sequence) const {
    StringSequence *self = (StringSequence *) (unsigned long) this;
    self->_referencers.push(sequence);
}

void SuperString::StringSequence::removeReferencer(SuperString::ReferenceStringSequence *sequence) const {
    StringSequence *self = (StringSequence *) (unsigned long) this;
    self->_referencers.remove(sequence);
}

std::size_t SuperString::StringSequence::freeingCost() const {
    std::size_t cost = 0;
    SingleLinkedList<ReferenceStringSequence *>::Node<ReferenceStringSequence *> *node = this->_referencers._head;
    while(node != NULL) {
        cost += node->_data->reconstructionCost(this);
        node = node->_next;
    }
    return cost;
}

void SuperString::StringSequence::reconstructReferencers() {
    SingleLinkedList<ReferenceStringSequence *>::Node<ReferenceStringSequence *> *node = this->_referencers._head;
    while(node != NULL) {
        node->_data->reconstruct(this);
        node = node->_next;
    }
}

bool SuperString::StringSequence::_substringMatches(std::size_t startIndex,
                                                                 SuperString other) const {
    if(other.isEmpty()) {
        return true;
    }
    std::size_t length = other.length();
    if(startIndex + length > this->length()) {
        return false;
    }
    for(int i = 0; i < length; i++) {
        if(this->codeUnitAt(i + startIndex).ok() != other.codeUnitAt(i).ok()) {
            return false;
        }
    }
    return true;
}

//*-- SuperString::ReferenceStringSequence (abstract|internal)
SuperString::ReferenceStringSequence::~ReferenceStringSequence() {
    // nothing go here
}

//*-- SuperString::ConstASCIISequence (internal)
SuperString::ConstASCIISequence::ConstASCIISequence(const Byte *bytes)
        : _bytes(bytes),
          _status(SuperString::ConstASCIISequence::Status::LengthNotComputed) {
    // nothing go here
}

SuperString::ConstASCIISequence::~ConstASCIISequence() {
    this->reconstructReferencers();
}

std::size_t SuperString::ConstASCIISequence::length() const /*override*/ {
    if(this->_status == Status::LengthNotComputed) {
        ConstASCIISequence *self = ((ConstASCIISequence *) ((std::size_t) this)); // to keep this method `const`
        self->_status = Status::LengthComputed;
        self->_length = SuperString::ASCII::length(this->_bytes);
    }
    return this->_length;
}

SuperString::Result<int, SuperString::Error>
SuperString::ConstASCIISequence::codeUnitAt(std::size_t index) const {
    if(index < this->length()) {
        return Result<int, SuperString::Error>(SuperString::ASCII::codeUnitAt(this->_bytes, index));
    }
    return Result<int, SuperString::Error>(Error::RangeError);
}

SuperString::Result<SuperString, SuperString::Error>
SuperString::ConstASCIISequence::substring(std::size_t startIndex,
                                           std::size_t endIndex) const {
    // TODO: General code, specify + repeated * times
    if(this->length() < startIndex || this->length() < endIndex) {
        return Result<SuperString, Error>(Error::RangeError);
    }
    SubstringSequence *sequence = new SubstringSequence(this, startIndex, endIndex);
    return Result<SuperString, Error>(SuperString(sequence));
}

bool SuperString::ConstASCIISequence::print(std::ostream &stream) const {
    SuperString::ASCII::print(stream, this->_bytes);
    return true;
}

bool SuperString::ConstASCIISequence::print(std::ostream &stream, std::size_t startIndex,
                                                         std::size_t endIndex) const {
    if(this->length() < startIndex || this->length() < endIndex) {
        return false;
    }
    SuperString::ASCII::print(stream, this->_bytes, startIndex, endIndex);
    return true;
}

SuperString SuperString::ConstASCIISequence::trim() const {
    Pair<std::size_t, std::size_t> indexes = SuperString::ASCII::trim(this->_bytes, this->_length);
    return this->substring(indexes.first(), indexes.second()).ok();
}

SuperString SuperString::ConstASCIISequence::trimLeft() const {
    return this->substring(SuperString::ASCII::trimLeft(this->_bytes), this->length()).ok();
}

SuperString SuperString::ConstASCIISequence::trimRight() const {
    return this->substring(0, SuperString::ASCII::trimRight(this->_bytes, this->length())).ok();
}

std::size_t SuperString::ConstASCIISequence::keepingCost() const {
    return sizeof(ConstASCIISequence);
}

void SuperString::ConstASCIISequence::doDelete() const {
    ConstASCIISequence *self = ((ConstASCIISequence *) (std::size_t) this);
    if(!self->isToBeDeleted()) {
        self->_status = Status::ToBeDestructed; // Just a trick, we don't want any more variable
        delete self;
    }
}

bool SuperString::ConstASCIISequence::isToBeDeleted() const {
    return this->_status == Status::ToBeDestructed;
}

//*-- SuperString::CopyASCIISequence (internal)
SuperString::CopyASCIISequence::CopyASCIISequence(const SuperString::Byte *bytes) {
    this->_length = SuperString::ASCII::length(bytes);
    this->_data = new Byte[this->_length + 1];
    std::copy_n(bytes, this->_length + 1, this->_data);
}

SuperString::CopyASCIISequence::CopyASCIISequence(const SuperString::ConstASCIISequence *sequence) {
    this->_length = sequence->length();
    this->_data = new Byte[this->_length + 1];
    std::copy_n(sequence->_bytes, this->_length + 1, this->_data);
}

SuperString::CopyASCIISequence::~CopyASCIISequence() {
    this->reconstructReferencers();
    delete this->_data;
}

std::size_t SuperString::CopyASCIISequence::length() const {
    return this->_length;
}

SuperString::Result<int, SuperString::Error> SuperString::CopyASCIISequence::codeUnitAt(
        std::size_t index) const {
    if(index < this->length()) {
        return Result<int, SuperString::Error>(SuperString::ASCII::codeUnitAt(this->_data, index));
    }
    return Result<int, SuperString::Error>(Error::RangeError);
}

SuperString::Result<SuperString, SuperString::Error>
SuperString::CopyASCIISequence::substring(std::size_t startIndex,
                                          std::size_t endIndex) const {
    // TODO: General code, specify + repeated * times
    if(this->length() < startIndex || this->length() < endIndex) {
        return Result<SuperString, Error>(Error::RangeError);
    }
    SubstringSequence *sequence = new SubstringSequence(this, startIndex, endIndex);
    return Result<SuperString, Error>(SuperString(sequence));
}

bool SuperString::CopyASCIISequence::print(std::ostream &stream) const {
    SuperString::ASCII::print(stream, this->_data);
    return true;
}

bool SuperString::CopyASCIISequence::print(std::ostream &stream, std::size_t startIndex,
                                                        std::size_t endIndex) const {
    if(this->length() < startIndex || this->length() < endIndex) {
        return false;
    }
    SuperString::ASCII::print(stream, this->_data, startIndex, endIndex);
    return true;
}

SuperString SuperString::CopyASCIISequence::trim() const {
    Pair<std::size_t, std::size_t> indexes = SuperString::ASCII::trim(this->_data, this->_length);
    return this->substring(indexes.first(), indexes.second()).ok();
}

SuperString SuperString::CopyASCIISequence::trimLeft() const {
    return this->substring(SuperString::ASCII::trimLeft(this->_data), this->length()).ok();
}

SuperString SuperString::CopyASCIISequence::trimRight() const {
    return this->substring(0, SuperString::ASCII::trimRight(this->_data, this->length())).ok();
}

std::size_t SuperString::CopyASCIISequence::keepingCost() const {
    std::size_t cost = sizeof(CopyASCIISequence);
    if(this->_data != NULL) {
        cost += this->length() + 1;
    }
    return cost;
}

void SuperString::CopyASCIISequence::doDelete() const {
    CopyASCIISequence *self = ((CopyASCIISequence *) (std::size_t) this);
    if(!self->isToBeDeleted()) {
        self->_length = 0; // Just a trick, we don't want any more variable
        delete self;
    }
}

bool SuperString::CopyASCIISequence::isToBeDeleted() const {
    return this->_length == 0;
}

//*-- SuperString::ConstUTF8Sequence (internal)
SuperString::ConstUTF8Sequence::ConstUTF8Sequence(const Byte *bytes)
        : _bytes(bytes),
          _status(SuperString::ConstUTF8Sequence::Status::LengthNotComputed) {
    // nothing go here
}

SuperString::ConstUTF8Sequence::~ConstUTF8Sequence() {
    this->reconstructReferencers();
}

std::size_t SuperString::ConstUTF8Sequence::length() const /*override*/ {
    if(this->_status == Status::LengthNotComputed) {
        ConstUTF8Sequence *self = ((ConstUTF8Sequence *) ((std::size_t) this)); // to keep this method `const`
        self->_status = Status::LengthComputed;
        self->_length = SuperString::UTF8::length(this->_bytes);
    }
    return this->_length;
}

SuperString::Result<int, SuperString::Error> SuperString::ConstUTF8Sequence::codeUnitAt(std::size_t index) const {
    return SuperString::UTF8::codeUnitAt(this->_bytes, index);
}

SuperString::Result<SuperString, SuperString::Error>
SuperString::ConstUTF8Sequence::substring(std::size_t startIndex,
                                          std::size_t endIndex) const {
    // TODO: General code, specify + repeated * times
    std::size_t length = this->length();
    if(length < startIndex || length < endIndex) {
        return Result<SuperString, Error>(Error::RangeError);
    }
    SubstringSequence *sequence = new SubstringSequence(this, startIndex, endIndex);
    return Result<SuperString, Error>(SuperString(sequence));
}

bool SuperString::ConstUTF8Sequence::print(std::ostream &stream) const {
    SuperString::UTF8::print(stream, this->_bytes);
    return true;
}

bool SuperString::ConstUTF8Sequence::print(std::ostream &stream, std::size_t startIndex,
                                                        std::size_t endIndex) const {
    std::size_t length = this->length();
    if(length < startIndex || length < endIndex) {
        return false;
    }
    SuperString::UTF8::print(stream, this->_bytes, startIndex, endIndex);
    return true;
}

SuperString SuperString::ConstUTF8Sequence::trim() const {
    // TODO: General code, specify
    std::size_t startIndex = 0;
    std::size_t endIndex = this->length();
    Result<int, Error> result = this->codeUnitAt(startIndex);
    while(result.isOk() && SuperString::isWhiteSpace(result.ok())) {
        result = this->codeUnitAt(++startIndex);
    }
    result = this->codeUnitAt(endIndex - 1);
    while(result.isOk() && SuperString::isWhiteSpace(result.ok())) {
        result = this->codeUnitAt(--endIndex - 1);
    }
    return this->substring(startIndex, endIndex).ok();
}

SuperString SuperString::ConstUTF8Sequence::trimLeft() const {
    // TODO: General code, specify
    std::size_t startIndex = 0;
    Result<int, Error> result = this->codeUnitAt(startIndex);
    while(result.isOk() && SuperString::isWhiteSpace(result.ok())) {
        result = this->codeUnitAt(++startIndex);
    }
    return this->substring(startIndex, this->length()).ok();
}

SuperString SuperString::ConstUTF8Sequence::trimRight() const {
    // TODO: General code, specify
    std::size_t endIndex = this->length();
    Result<int, Error> result = this->codeUnitAt(endIndex - 1);
    while(result.isOk() && SuperString::isWhiteSpace(result.ok())) {
        result = this->codeUnitAt(--endIndex - 1);
    }
    return this->substring(0, endIndex).ok();
}

std::size_t SuperString::ConstUTF8Sequence::keepingCost() const {
    return sizeof(ConstUTF8Sequence);
}

void SuperString::ConstUTF8Sequence::doDelete() const {
    ConstUTF8Sequence *self = ((ConstUTF8Sequence *) (std::size_t) this);
    if(!self->isToBeDeleted()) {
        self->_status = Status::ToBeDestructed; // Just a trick, we don't want any more variable
        delete self;
    }
}

bool SuperString::ConstUTF8Sequence::isToBeDeleted() const {
    return this->_status == Status::ToBeDestructed;
}

//*-- SuperString::CopyUTF8Sequence (internal)
SuperString::CopyUTF8Sequence::CopyUTF8Sequence(const SuperString::Byte *bytes) {
    Pair<std::size_t, std::size_t> lengthAndMemoryLength = SuperString::UTF8::lengthAndMemoryLength(bytes);
    this->_length = lengthAndMemoryLength.first();
    this->_memoryLength = lengthAndMemoryLength.second();
    this->_data = new Byte[this->_memoryLength];
    std::copy_n(bytes, this->_memoryLength, this->_data);
}

SuperString::CopyUTF8Sequence::CopyUTF8Sequence(const SuperString::ConstUTF8Sequence *sequence) {
    Pair<std::size_t, std::size_t> lengthAndMemoryLength = SuperString::UTF8::lengthAndMemoryLength(sequence->_bytes);
    this->_length = lengthAndMemoryLength.first();
    this->_memoryLength = lengthAndMemoryLength.second();
    this->_data = new Byte[this->_memoryLength];
    std::copy_n(sequence->_bytes, this->_memoryLength, this->_data);
}

SuperString::CopyUTF8Sequence::~CopyUTF8Sequence() {
    this->reconstructReferencers();
    delete this->_data;
}

std::size_t SuperString::CopyUTF8Sequence::length() const {
    return this->_length;
}

SuperString::Result<int, SuperString::Error> SuperString::CopyUTF8Sequence::codeUnitAt(std::size_t index) const {
    if(index < this->length()) {
        return Result<int, Error>(SuperString::UTF8::codeUnitAt(this->_data, index));
    }
    return Result<int, SuperString::Error>(Error::RangeError);
}

SuperString::Result<SuperString, SuperString::Error>
SuperString::CopyUTF8Sequence::substring(std::size_t startIndex, std::size_t endIndex) const {
    // TODO: General code, specify + repeated * times
    if(this->length() < startIndex || this->length() < endIndex) {
        return Result<SuperString, Error>(Error::RangeError);
    }
    SubstringSequence *sequence = new SubstringSequence(this, startIndex, endIndex);
    return Result<SuperString, Error>(SuperString(sequence));
}

bool SuperString::CopyUTF8Sequence::print(std::ostream &stream) const {
    SuperString::UTF8::print(stream, this->_data);
    return true;
}

bool SuperString::CopyUTF8Sequence::print(std::ostream &stream, std::size_t startIndex,
                                                       std::size_t endIndex) const {
    std::size_t length = this->length();
    if(length < startIndex || length < endIndex) {
        return false;
    }
    SuperString::UTF8::print(stream, this->_data, startIndex, endIndex);
    return true;
}

SuperString SuperString::CopyUTF8Sequence::trim() const {
    // TODO: General code, specify
    std::size_t startIndex = 0;
    std::size_t endIndex = this->length();
    Result<int, Error> result = this->codeUnitAt(startIndex);
    while(result.isOk() && SuperString::isWhiteSpace(result.ok())) {
        result = this->codeUnitAt(++startIndex);
    }
    result = this->codeUnitAt(endIndex - 1);
    while(result.isOk() && SuperString::isWhiteSpace(result.ok())) {
        result = this->codeUnitAt(--endIndex - 1);
    }
    return this->substring(startIndex, endIndex).ok();
}

SuperString SuperString::CopyUTF8Sequence::trimLeft() const {
    // TODO: General code, specify
    std::size_t startIndex = 0;
    Result<int, Error> result = this->codeUnitAt(startIndex);
    while(result.isOk() && SuperString::isWhiteSpace(result.ok())) {
        result = this->codeUnitAt(++startIndex);
    }
    return this->substring(startIndex, this->length()).ok();
}

SuperString SuperString::CopyUTF8Sequence::trimRight() const {
    // TODO: General code, specify
    std::size_t endIndex = this->length();
    Result<int, Error> result = this->codeUnitAt(endIndex - 1);
    while(result.isOk() && SuperString::isWhiteSpace(result.ok())) {
        result = this->codeUnitAt(--endIndex - 1);
    }
    return this->substring(0, endIndex).ok();
}

std::size_t SuperString::CopyUTF8Sequence::keepingCost() const {
    std::size_t cost = sizeof(CopyUTF8Sequence) + this->_memoryLength;
    return cost;
}

void SuperString::CopyUTF8Sequence::doDelete() const {
    CopyUTF8Sequence *self = ((CopyUTF8Sequence *) (std::size_t) this);
    if(!self->isToBeDeleted()) {
        self->_length = 0; // Just a trick, we don't want any more variable
        delete self;
    }
}

bool SuperString::CopyUTF8Sequence::isToBeDeleted() const {
    return this->_length == 0;
}

//*-- ConstUTF16BESequence (internal)
SuperString::ConstUTF16BESequence::ConstUTF16BESequence(const SuperString::Byte *bytes)
        : _bytes(bytes),
          _status(SuperString::ConstUTF16BESequence::Status::LengthNotComputed) {
    // nothing go here
}

SuperString::ConstUTF16BESequence::~ConstUTF16BESequence() {
    this->reconstructReferencers();
}

std::size_t SuperString::ConstUTF16BESequence::length() const /*override*/ {
    if(this->_status == Status::LengthNotComputed) {
        ConstUTF16BESequence *self = ((ConstUTF16BESequence *) ((std::size_t) this)); // to keep this method `const`
        self->_status = Status::LengthComputed;
        self->_length = SuperString::UTF16BE::length(this->_bytes);
    }
    return this->_length;
}

SuperString::Result<int, SuperString::Error> SuperString::ConstUTF16BESequence::codeUnitAt(
        std::size_t index) const {
    if(index < this->length()) {
        return SuperString::UTF16BE::codeUnitAt(this->_bytes, index);
    }
    return Result<int, SuperString::Error>(Error::RangeError);
}

SuperString::Result<SuperString, SuperString::Error>
SuperString::ConstUTF16BESequence::substring(std::size_t startIndex,
                                             std::size_t endIndex) const {
    // TODO: General code, specify + repeated * times
    if(this->length() < startIndex || this->length() < endIndex) {
        return Result<SuperString, Error>(Error::RangeError);
    }
    SubstringSequence *sequence = new SubstringSequence(this, startIndex, endIndex);
    return Result<SuperString, Error>(SuperString(sequence));
}

bool SuperString::ConstUTF16BESequence::print(std::ostream &stream) const {
    SuperString::UTF16BE::print(stream, this->_bytes, this->length());
    return true;
}

bool SuperString::ConstUTF16BESequence::print(std::ostream &stream, std::size_t startIndex,
                                                           std::size_t endIndex) const {
    std::size_t length = this->length();
    if(length < startIndex || length < endIndex) {
        return false;
    }
    SuperString::UTF16BE::print(stream, this->_bytes, startIndex, endIndex);
    return true;
}

SuperString SuperString::ConstUTF16BESequence::trim() const {
    // TODO: General code, specify
    std::size_t startIndex = 0;
    std::size_t endIndex = this->length();
    Result<int, Error> result = this->codeUnitAt(startIndex);
    while(result.isOk() && SuperString::isWhiteSpace(result.ok())) {
        result = this->codeUnitAt(++startIndex);
    }
    result = this->codeUnitAt(endIndex - 1);
    while(result.isOk() && SuperString::isWhiteSpace(result.ok())) {
        result = this->codeUnitAt(--endIndex - 1);
    }
    return this->substring(startIndex, endIndex).ok();
}

SuperString SuperString::ConstUTF16BESequence::trimLeft() const {
    // TODO: General code, specify
    std::size_t startIndex = 0;
    Result<int, Error> result = this->codeUnitAt(startIndex);
    while(result.isOk() && SuperString::isWhiteSpace(result.ok())) {
        result = this->codeUnitAt(++startIndex);
    }
    return this->substring(startIndex, this->length()).ok();
}

SuperString SuperString::ConstUTF16BESequence::trimRight() const {
    // TODO: General code, specify
    std::size_t endIndex = this->length();
    Result<int, Error> result = this->codeUnitAt(endIndex - 1);
    while(result.isOk() && SuperString::isWhiteSpace(result.ok())) {
        result = this->codeUnitAt(--endIndex - 1);
    }
    return this->substring(0, endIndex).ok();
}

std::size_t SuperString::ConstUTF16BESequence::keepingCost() const {
    return sizeof(ConstUTF16BESequence);
}

void SuperString::ConstUTF16BESequence::doDelete() const {
    ConstUTF16BESequence *self = ((ConstUTF16BESequence *) (std::size_t) this);
    if(!self->isToBeDeleted()) {
        self->_status = Status::ToBeDestructed; // Just a trick, we don't want any more variable
        delete self;
    }
}

bool SuperString::ConstUTF16BESequence::isToBeDeleted() const {
    return this->_status == Status::ToBeDestructed;
}

//*-- SuperString::CopyUTF16BESequence (internal)
SuperString::CopyUTF16BESequence::CopyUTF16BESequence(const SuperString::Byte *bytes) {
    Pair<std::size_t, std::size_t> lengthAndMemoryLength = SuperString::UTF16BE::lengthAndMemoryLength(bytes);
    this->_length = lengthAndMemoryLength.first();
    this->_memoryLength = lengthAndMemoryLength.second();
    this->_data = new Byte[this->_memoryLength];
    std::copy_n(bytes, this->_memoryLength, this->_data);
}

SuperString::CopyUTF16BESequence::CopyUTF16BESequence(const SuperString::ConstUTF16BESequence *sequence) {
    Pair<std::size_t, std::size_t> lengthAndMemoryLength = SuperString::UTF16BE::lengthAndMemoryLength(sequence->_bytes);
    this->_length = lengthAndMemoryLength.first();
    this->_memoryLength = lengthAndMemoryLength.second();
    this->_data = new Byte[this->_memoryLength];
    std::copy_n(sequence->_bytes, this->_memoryLength, this->_data);
}

SuperString::CopyUTF16BESequence::~CopyUTF16BESequence() {
    this->reconstructReferencers();
    delete this->_data;
}

std::size_t SuperString::CopyUTF16BESequence::length() const {
    return this->_length;
}

SuperString::Result<int, SuperString::Error>
SuperString::CopyUTF16BESequence::codeUnitAt(std::size_t index) const {
    if(index < this->length()) {
        return SuperString::UTF16BE::codeUnitAt(this->_data, index);
    }
    return Result<int, SuperString::Error>(Error::RangeError);
}

SuperString::Result<SuperString, SuperString::Error>
SuperString::CopyUTF16BESequence::substring(std::size_t startIndex, std::size_t endIndex) const {
    // TODO: General code, specify + repeated * times
    if(this->length() < startIndex || this->length() < endIndex) {
        return Result<SuperString, Error>(Error::RangeError);
    }
    SubstringSequence *sequence = new SubstringSequence(this, startIndex, endIndex);
    return Result<SuperString, Error>(SuperString(sequence));
}

bool SuperString::CopyUTF16BESequence::print(std::ostream &stream) const {
    SuperString::UTF16BE::print(stream, this->_data, this->length());
    return true;
}

bool SuperString::CopyUTF16BESequence::print(std::ostream &stream, std::size_t startIndex,
                                                          std::size_t endIndex) const {
    std::size_t length = this->length();
    if(length < startIndex || length < endIndex) {
        return false;
    }
    SuperString::UTF16BE::print(stream, this->_data, startIndex, endIndex);
    return true;
}

SuperString SuperString::CopyUTF16BESequence::trim() const {
    // TODO: General code, specify
    std::size_t startIndex = 0;
    std::size_t endIndex = this->length();
    Result<int, Error> result = this->codeUnitAt(startIndex);
    while(result.isOk() && SuperString::isWhiteSpace(result.ok())) {
        result = this->codeUnitAt(++startIndex);
    }
    result = this->codeUnitAt(endIndex - 1);
    while(result.isOk() && SuperString::isWhiteSpace(result.ok())) {
        result = this->codeUnitAt(--endIndex - 1);
    }
    return this->substring(startIndex, endIndex).ok();
}

SuperString SuperString::CopyUTF16BESequence::trimLeft() const {
    // TODO: General code, specify
    std::size_t startIndex = 0;
    Result<int, Error> result = this->codeUnitAt(startIndex);
    while(result.isOk() && SuperString::isWhiteSpace(result.ok())) {
        result = this->codeUnitAt(++startIndex);
    }
    return this->substring(startIndex, this->length()).ok();
}

SuperString SuperString::CopyUTF16BESequence::trimRight() const {
    // TODO: General code, specify
    std::size_t endIndex = this->length();
    Result<int, Error> result = this->codeUnitAt(endIndex - 1);
    while(result.isOk() && SuperString::isWhiteSpace(result.ok())) {
        result = this->codeUnitAt(--endIndex - 1);
    }
    return this->substring(0, endIndex).ok();
}

std::size_t SuperString::CopyUTF16BESequence::keepingCost() const {
    std::size_t cost = sizeof(CopyUTF16BESequence) + this->_memoryLength;
    return cost;
}

void SuperString::CopyUTF16BESequence::doDelete() const {
    CopyUTF16BESequence *self = ((CopyUTF16BESequence *) (std::size_t) this);
    if(!self->isToBeDeleted()) {
        self->_length = 0; // Just a trick, we don't want any more variable
        delete self;
    }
}

bool SuperString::CopyUTF16BESequence::isToBeDeleted() const {
    return this->_length == 0;
}

//*-- SuperString::ConstUTF32Sequence (internal)
SuperString::ConstUTF32Sequence::ConstUTF32Sequence(const SuperString::Byte *bytes)
        : _bytes(((const int *) bytes)),
          _status(SuperString::ConstUTF32Sequence::Status::LengthNotComputed) {
    // nothing go here
}

SuperString::ConstUTF32Sequence::~ConstUTF32Sequence() {
    this->reconstructReferencers();
}

std::size_t SuperString::ConstUTF32Sequence::length() const /*override*/ {
    if(this->_status == Status::LengthNotComputed) {
        ConstUTF32Sequence *self = ((ConstUTF32Sequence *) ((std::size_t) this)); // to keep this method `const`
        self->_status = Status::LengthComputed;
        self->_length = SuperString::UTF32::length(((const Byte *) this->_bytes));
    }
    return this->_length;
}

SuperString::Result<int, SuperString::Error>
SuperString::ConstUTF32Sequence::codeUnitAt(std::size_t index) const {
    if(index < this->length()) {
        return Result<int, SuperString::Error>(SuperString::UTF32::codeUnitAt(((const Byte *) this->_bytes), index));
    }
    return Result<int, SuperString::Error>(Error::RangeError);
}

SuperString::Result<SuperString, SuperString::Error>
SuperString::ConstUTF32Sequence::substring(std::size_t startIndex,
                                           std::size_t endIndex) const {
    // TODO: General code, specify + repeated * times
    if(this->length() < startIndex || this->length() < endIndex) {
        return Result<SuperString, Error>(Error::RangeError);
    }
    SubstringSequence *sequence = new SubstringSequence(this, startIndex, endIndex);
    return Result<SuperString, Error>(SuperString(sequence));
}

bool SuperString::ConstUTF32Sequence::print(std::ostream &stream) const {
    SuperString::UTF32::print(stream, ((const Byte *) this->_bytes));
    return true;
}

bool SuperString::ConstUTF32Sequence::print(std::ostream &stream, std::size_t startIndex,
                                                         std::size_t endIndex) const {
    if(this->length() < startIndex || this->length() < endIndex) {
        return false;
    }
    SuperString::UTF32::print(stream, ((Byte *) this->_bytes), startIndex, endIndex);
    return true;
}

SuperString SuperString::ConstUTF32Sequence::trim() const {
    Pair<std::size_t, std::size_t> indexes = SuperString::UTF32::trim(((Byte *) this->_bytes), this->_length);
    return this->substring(indexes.first(), indexes.second()).ok();
}

SuperString SuperString::ConstUTF32Sequence::trimLeft() const {
    return this->substring(SuperString::UTF32::trimLeft(((Byte *) this->_bytes)), this->length()).ok();
}

SuperString SuperString::ConstUTF32Sequence::trimRight() const {
    return this->substring(0, SuperString::UTF32::trimRight(((Byte *) this->_bytes), this->length())).ok();
}

std::size_t SuperString::ConstUTF32Sequence::keepingCost() const {
    return sizeof(ConstUTF32Sequence);
}

void SuperString::ConstUTF32Sequence::doDelete() const {
    ConstUTF32Sequence *self = ((ConstUTF32Sequence *) (std::size_t) this);
    if(!self->isToBeDeleted()) {
        self->_status = Status::ToBeDestructed; // Just a trick, we don't want any more variable
        delete self;
    }
}

bool SuperString::ConstUTF32Sequence::isToBeDeleted() const {
    return this->_status == Status::ToBeDestructed;
}

//*-- SuperString::CopyUTF32Sequence (internal)
SuperString::CopyUTF32Sequence::CopyUTF32Sequence(const SuperString::Byte *bytes) {
    this->_length = SuperString::UTF32::length(bytes);
    this->_data = new int[this->_length + 1];
    std::copy_n(bytes, this->_length + 1, this->_data);
}

SuperString::CopyUTF32Sequence::CopyUTF32Sequence(const SuperString::ConstUTF32Sequence *sequence) {
    this->_length = SuperString::UTF32::length(((const Byte *) sequence->_bytes));
    this->_data = new int[this->_length + 1];
    std::copy_n(sequence->_bytes, this->_length + 1, this->_data);
}

SuperString::CopyUTF32Sequence::~CopyUTF32Sequence() {
    this->reconstructReferencers();
    delete this->_data;
}

std::size_t SuperString::CopyUTF32Sequence::length() const {
    return this->_length;
}

SuperString::Result<int, SuperString::Error> SuperString::CopyUTF32Sequence::codeUnitAt(
        std::size_t index) const {
    if(index < this->length()) {
        return Result<int, SuperString::Error>(SuperString::UTF32::codeUnitAt(((const Byte *) this->_data), index));
    }
    return Result<int, SuperString::Error>(Error::RangeError);
}

SuperString::Result<SuperString, SuperString::Error>
SuperString::CopyUTF32Sequence::substring(std::size_t startIndex,
                                          std::size_t endIndex) const {
    // TODO: General code, specify + repeated * times
    if(this->length() < startIndex || this->length() < endIndex) {
        return Result<SuperString, Error>(Error::RangeError);
    }
    SubstringSequence *sequence = new SubstringSequence(this, startIndex, endIndex);
    return Result<SuperString, Error>(SuperString(sequence));
}

bool SuperString::CopyUTF32Sequence::print(std::ostream &stream) const {
    SuperString::UTF32::print(stream, ((const Byte *) this->_data));
    return true;
}

bool SuperString::CopyUTF32Sequence::print(std::ostream &stream, std::size_t startIndex,
                                                        std::size_t endIndex) const {
    if(this->length() < startIndex || this->length() < endIndex) {
        return false;
    }
    SuperString::UTF32::print(stream, ((const Byte *) this->_data), startIndex, endIndex);
    return true;
}

SuperString SuperString::CopyUTF32Sequence::trim() const {
    Pair<std::size_t, std::size_t> indexes = SuperString::UTF32::trim(((const Byte *) this->_data), this->_length);
    return this->substring(indexes.first(), indexes.second()).ok();
}

SuperString SuperString::CopyUTF32Sequence::trimLeft() const {
    return this->substring(SuperString::UTF32::trimLeft(((const Byte *) this->_data)), this->length()).ok();
}

SuperString SuperString::CopyUTF32Sequence::trimRight() const {
    return this->substring(0, SuperString::UTF32::trimRight(((const Byte *) this->_data), this->length())).ok();
}

std::size_t SuperString::CopyUTF32Sequence::keepingCost() const {
    std::size_t cost = sizeof(CopyASCIISequence);
    if(this->_data != NULL) {
        cost += this->length() + 1;
    }
    return cost;
}

void SuperString::CopyUTF32Sequence::doDelete() const {
    CopyUTF32Sequence *self = ((CopyUTF32Sequence *) (std::size_t) this);
    if(!self->isToBeDeleted()) {
        self->_length = 0; // Just a trick, we don't want any more variable
        delete self;
    }
}

bool SuperString::CopyUTF32Sequence::isToBeDeleted() const {
    return this->_length == 0;
}

//*-- SuperString::SubstringSequence (internal)
SuperString::SubstringSequence::SubstringSequence(const StringSequence *sequence, std::size_t startIndex,
                                                  std::size_t endIndex) {
    this->_kind = Kind::SUBSTRING;
    this->_container._substring._sequence = sequence;
    this->_container._substring._startIndex = startIndex;
    this->_container._substring._endIndex = endIndex;
    this->_container._substring._sequence->addReferencer(this);
}

SuperString::SubstringSequence::~SubstringSequence() {
    switch(this->kind()) {
        case Kind::SUBSTRING:
            this->_container._substring._sequence->removeReferencer(this);
            if(this->_container._substring._sequence->refCount() == 0 &&
               this->_container._substring._sequence->freeingCost() <
               this->_container._substring._sequence->keepingCost()) {
                this->_container._substring._sequence->doDelete();
            }
            break;
        case Kind::RECONSTRUCTED:
            delete this->_container._reconstructed._data;
            break;
    }
}

SuperString::SubstringSequence::Kind SuperString::SubstringSequence::kind() const {
    return (Kind) (((char) this->_kind) & 0b01111111);
}

std::size_t SuperString::SubstringSequence::length() const /*override*/ {
    switch(this->kind()) {
        case Kind::SUBSTRING:
            return this->_container._substring._endIndex - this->_container._substring._startIndex;
        case Kind::RECONSTRUCTED:
            return this->_container._reconstructed._length;
    }
}

SuperString::Result<int, SuperString::Error> SuperString::SubstringSequence::codeUnitAt(
        std::size_t index) const {
    if(index < this->length()) {
        switch(this->kind()) {
            case Kind::SUBSTRING:
                return this->_container._substring._sequence->codeUnitAt(
                        this->_container._substring._startIndex + index);
            case Kind::RECONSTRUCTED:
                return Result<int, SuperString::Error>(*(this->_container._reconstructed._data + index));
        }
    }
    return Result<int, SuperString::Error>(Error::RangeError);
}

SuperString::Result<SuperString, SuperString::Error>
SuperString::SubstringSequence::substring(std::size_t startIndex, std::size_t endIndex) const {
    switch(this->kind()) {
        case Kind::SUBSTRING:
            if((this->_container._substring._sequence->length() <
                (this->_container._substring._startIndex + startIndex)) ||
               (this->_container._substring._sequence->length() <
                (this->_container._substring._startIndex + endIndex))) {
                return Result<SuperString, Error>(Error::RangeError);
            } else {
                return Result<SuperString, Error>(
                        SuperString(new SubstringSequence(this->_container._substring._sequence,
                                                          this->_container._substring._startIndex +
                                                          startIndex,
                                                          this->_container._substring._startIndex +
                                                          endIndex)));
            }
        case Kind::RECONSTRUCTED:
            // TODO: General code, specify + repeated * times
            if(this->length() < startIndex || this->length() < endIndex) {
                return Result<SuperString, Error>(Error::RangeError);
            }
            SubstringSequence *sequence = new SubstringSequence(this, startIndex, endIndex);
            return Result<SuperString, Error>(SuperString(sequence));
    }
}

bool SuperString::SubstringSequence::print(std::ostream &stream) const {
    switch(this->kind()) {
        case Kind::SUBSTRING:
            return this->_container._substring._sequence->print(stream, this->_container._substring._startIndex,
                                                                this->_container._substring._endIndex);
        case Kind::RECONSTRUCTED:
            SuperString::UTF32::print(stream, ((Byte *) this->_container._reconstructed._data));
            return true;
    }
}

bool SuperString::SubstringSequence::print(std::ostream &stream, std::size_t startIndex,
                                                        std::size_t endIndex) const {
    switch(this->kind()) {
        case Kind::SUBSTRING:
            return this->_container._substring._sequence->print(stream,
                                                                this->_container._substring._startIndex + startIndex,
                                                                this->_container._substring._startIndex + endIndex);
        case Kind::RECONSTRUCTED:
            SuperString::UTF32::print(stream, ((Byte *) this->_container._reconstructed._data), startIndex, endIndex);
            return true;
    }
}

SuperString SuperString::SubstringSequence::trim() const {
    // TODO: General code, specify
    std::size_t startIndex = 0;
    std::size_t endIndex = this->length();
    Result<int, Error> result = this->codeUnitAt(startIndex);
    while(result.isOk() && SuperString::isWhiteSpace(result.ok())) {
        result = this->codeUnitAt(++startIndex);
    }
    result = this->codeUnitAt(endIndex - 1);
    while(result.isOk() && SuperString::isWhiteSpace(result.ok())) {
        result = this->codeUnitAt(--endIndex - 1);
    }
    return this->substring(startIndex, endIndex).ok();
}

SuperString SuperString::SubstringSequence::trimLeft() const {
    // TODO: General code, specify
    std::size_t startIndex = 0;
    Result<int, Error> result = this->codeUnitAt(startIndex);
    while(result.isOk() && SuperString::isWhiteSpace(result.ok())) {
        result = this->codeUnitAt(++startIndex);
    }
    return this->substring(startIndex, this->length()).ok();
}

SuperString SuperString::SubstringSequence::trimRight() const {
    // TODO: General code, specify
    std::size_t endIndex = this->length();
    Result<int, Error> result = this->codeUnitAt(endIndex - 1);
    while(result.isOk() && SuperString::isWhiteSpace(result.ok())) {
        result = this->codeUnitAt(--endIndex - 1);
    }
    return this->substring(0, endIndex).ok();
}

std::size_t SuperString::SubstringSequence::keepingCost() const {
    switch(this->kind()) {
        case Kind::SUBSTRING:
            return sizeof(SubstringSequence) + this->_container._substring._sequence->keepingCost();
        case Kind::RECONSTRUCTED:
            return sizeof(SubstringSequence) + this->_container._reconstructed._length * sizeof(int);
    }
}

std::size_t SuperString::SubstringSequence::reconstructionCost(const StringSequence *sequence) const {
    if(this->kind() == Kind::SUBSTRING) {
        return sizeof(SubstringSequence) +
               (this->_container._substring._endIndex - this->_container._substring._startIndex) * sizeof(int);
    }
    return 0;
}

void SuperString::SubstringSequence::reconstruct(const StringSequence *sequence) const {
    SubstringSequence *self = ((SubstringSequence *) ((std::size_t) this));
    if(self->kind() == Kind::SUBSTRING) {
        struct SubstringMetaInfo old = self->_container._substring;
        struct ReconstructedMetaInfo nw;
        nw._length = old._endIndex - old._startIndex;
        nw._data = new int[nw._length];
        for(std::size_t i = 0; i < nw._length; i++) {
            nw._data[i] = old._sequence->codeUnitAt(old._startIndex + i).ok();
        }
        old._sequence->removeReferencer(self);
        if(old._sequence->refCount() == 0 && old._sequence->freeingCost() < old._sequence->keepingCost()) {
            old._sequence->doDelete();
        }
        self->_kind = Kind::RECONSTRUCTED;
        self->_container._reconstructed = nw;
    }
}

void SuperString::SubstringSequence::doDelete() const {
    SubstringSequence *self = ((SubstringSequence *) (std::size_t) this);
    if(!self->isToBeDeleted()) {
        self->_kind = (Kind) (((char) self->kind()) + 0b10000000); // Just a trick, we don't want any more variable
        delete self;
    }
}

bool SuperString::SubstringSequence::isToBeDeleted() const {
    return (((char) this->_kind) & 0b10000000) == 0b10000000;
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
    this->reconstructReferencers();
    switch(this->kind()) {
        case Kind::CONCATENATION:
            this->_container._concatenation._left->removeReferencer(this);
            if(this->_container._concatenation._left->refCount() == 0 &&
               this->_container._concatenation._left->freeingCost() <
               this->_container._concatenation._left->keepingCost()) {
                this->_container._concatenation._left->doDelete();
            }
            this->_container._concatenation._right->removeReferencer(this);
            if(this->_container._concatenation._right->refCount() == 0 &&
               this->_container._concatenation._right->freeingCost() <
               this->_container._concatenation._right->keepingCost()) {
                this->_container._concatenation._right->doDelete();
            }
            break;
        case Kind::LEFTRECONSTRUCTED:
            delete this->_container._leftReconstructed._leftData;
            this->_container._leftReconstructed._right->removeReferencer(this);
            if(this->_container._leftReconstructed._right->refCount() == 0 &&
               this->_container._leftReconstructed._right->freeingCost() <
               this->_container._leftReconstructed._right->keepingCost()) {
                this->_container._leftReconstructed._right->doDelete();
            }
            break;
        case Kind::RIGHTRECONSTRUCTED:
            delete this->_container._rightReconstructed._rightData;
            this->_container._rightReconstructed._left->removeReferencer(this);
            if(this->_container._rightReconstructed._left->refCount() == 0 &&
               this->_container._rightReconstructed._left->freeingCost() <
               this->_container._rightReconstructed._left->keepingCost()) {
                this->_container._rightReconstructed._left->doDelete();
            }
            break;
        case Kind::RECONSTRUCTED:
            delete this->_container._reconstructed._data;
    }
}

SuperString::ConcatenationSequence::Kind SuperString::ConcatenationSequence::kind() const {
    return (Kind) (((char) this->_kind) & 0b01111111);
}

std::size_t SuperString::ConcatenationSequence::length() const {
    switch(this->kind()) {
        case Kind::CONCATENATION:
            return this->_container._concatenation._left->length() + this->_container._concatenation._right->length();
        case Kind::LEFTRECONSTRUCTED:
            return this->_container._leftReconstructed._leftLength +
                   this->_container._leftReconstructed._right->length();
        case Kind::RIGHTRECONSTRUCTED:
            return this->_container._rightReconstructed._rightLength +
                   this->_container._rightReconstructed._left->length();
        case Kind::RECONSTRUCTED:
            return this->_container._reconstructed._length;
    }
}

SuperString::Result<int, SuperString::Error>
SuperString::ConcatenationSequence::codeUnitAt(std::size_t index) const {
    switch(this->kind()) {
        case Kind::CONCATENATION:
            if(index < this->_container._concatenation._left->length()) {
                return Result<int, Error>(this->_container._concatenation._left->codeUnitAt(index));
            } else if((index - this->_container._concatenation._left->length()) <
                      this->_container._concatenation._right->length()) {
                return Result<int, Error>(this->_container._concatenation._right->codeUnitAt(
                        index - this->_container._concatenation._left->length()));
            }
            break;
        case Kind::LEFTRECONSTRUCTED:
            if(index < this->_container._leftReconstructed._leftLength) {
                return Result<int, Error>(this->_container._leftReconstructed._leftData[index]);
            } else if((index - this->_container._leftReconstructed._leftLength) <
                      this->_container._leftReconstructed._right->length()) {
                return Result<int, Error>(this->_container._leftReconstructed._right->codeUnitAt(
                        index - this->_container._leftReconstructed._leftLength));
            }
            break;
        case Kind::RIGHTRECONSTRUCTED:
            if(index < this->_container._rightReconstructed._left->length()) {
                return Result<int, Error>(this->_container._rightReconstructed._left->codeUnitAt(index));
            } else if((index - this->_container._rightReconstructed._left->length()) <
                      this->_container._rightReconstructed._rightLength) {
                return Result<int, Error>(
                        this->_container._rightReconstructed._rightData[
                                index - this->_container._rightReconstructed._left->length()
                        ]);
            }
            break;
        case Kind::RECONSTRUCTED:
            if(index < this->_container._reconstructed._length) {
                return Result<int, Error>(this->_container._reconstructed._data[index]);
            }
    }
    return Result<int, Error>(Error::RangeError);
}

SuperString::Result<SuperString, SuperString::Error>
SuperString::ConcatenationSequence::substring(std::size_t startIndex,
                                              std::size_t endIndex) const {
    if(this->length() < startIndex || this->length() < endIndex) {
        return Result<SuperString, Error>(Error::RangeError);
    }
    SubstringSequence *sequence = new SubstringSequence(this, startIndex, endIndex);
    return Result<SuperString, Error>(SuperString(sequence));
}

bool SuperString::ConcatenationSequence::print(std::ostream &stream) const {
    bool isOk = true;
    switch(this->kind()) {
        case Kind::CONCATENATION:
            isOk &= this->_container._concatenation._left->print(stream);
            isOk &= this->_container._concatenation._right->print(stream);
            break;
        case Kind::LEFTRECONSTRUCTED:
            SuperString::UTF32::print(stream, (const Byte *) this->_container._leftReconstructed._leftData);
            isOk &= this->_container._leftReconstructed._right->print(stream);
            break;
        case Kind::RIGHTRECONSTRUCTED:
            isOk &= this->_container._rightReconstructed._left->print(stream);
            SuperString::UTF32::print(stream, (const Byte *) this->_container._rightReconstructed._rightData);
            break;
        case Kind::RECONSTRUCTED:
            SuperString::UTF32::print(stream, (const Byte *) this->_container._reconstructed._data);
            break;
    }
    return isOk;
}

bool SuperString::ConcatenationSequence::print(std::ostream &stream, std::size_t startIndex,
                                                            std::size_t endIndex) const {
    bool isOk = true;
    switch(this->kind()) {
        case Kind::CONCATENATION:
            if(startIndex < this->_container._concatenation._left->length()) {
                if(endIndex < this->_container._concatenation._left->length()) {
                    isOk &= this->_container._concatenation._left->print(stream, startIndex, endIndex);
                } else {
                    isOk &= this->_container._concatenation._left->print(stream, startIndex,
                                                                         this->_container._concatenation._left->length());
                    isOk &= this->_container._concatenation._right->print(stream, 0,
                                                                          endIndex -
                                                                          this->_container._concatenation._left->length());
                }
            } else {
                if((endIndex - this->_container._concatenation._left->length()) <
                   this->_container._concatenation._right->length()) {
                    isOk &= this->_container._concatenation._right->print(stream, startIndex -
                                                                                  this->_container._concatenation._left->length(),
                                                                          endIndex -
                                                                          this->_container._concatenation._left->length());
                }
            }
            break;
        case Kind::LEFTRECONSTRUCTED:
            if(startIndex < this->_container._leftReconstructed._leftLength) {
                if(endIndex < this->_container._leftReconstructed._leftLength) {
                    SuperString::UTF32::print(stream, (const Byte *) this->_container._leftReconstructed._leftData,
                                              startIndex, endIndex);
                } else {
                    SuperString::UTF32::print(stream, (const Byte *) this->_container._leftReconstructed._leftData,
                                              startIndex, this->_container._leftReconstructed._leftLength);
                    isOk &= this->_container._leftReconstructed._right->print(stream, 0,
                                                                              endIndex -
                                                                              this->_container._leftReconstructed._leftLength);
                }
            } else {
                if((endIndex - this->_container._leftReconstructed._leftLength) <
                   this->_container._leftReconstructed._right->length()) {
                    isOk &= this->_container._leftReconstructed._right->print(stream, startIndex -
                                                                                      this->_container._leftReconstructed._leftLength,
                                                                              endIndex -
                                                                              this->_container._leftReconstructed._leftLength);
                }
            }
            break;
        case Kind::RIGHTRECONSTRUCTED:
            if(startIndex < this->_container._rightReconstructed._left->length()) {
                if(endIndex < this->_container._rightReconstructed._left->length()) {
                    isOk &= this->_container._rightReconstructed._left->print(stream, startIndex, endIndex);
                } else {
                    isOk &= this->_container._rightReconstructed._left->print(stream, startIndex,
                                                                      this->_container._rightReconstructed._left->length());
                    SuperString::UTF32::print(stream, (const Byte *) this->_container._rightReconstructed._rightData, 0,
                                              endIndex - this->_container._rightReconstructed._left->length());
                }
            } else {
                if((endIndex - this->_container._rightReconstructed._left->length()) <
                   this->_container._rightReconstructed._rightLength) {
                    SuperString::UTF32::print(stream, (const Byte *) this->_container._rightReconstructed._rightData,
                                              startIndex - this->_container._rightReconstructed._left->length(),
                                              endIndex - this->_container._rightReconstructed._left->length());
                }
            }
            break;
        case Kind::RECONSTRUCTED:
            SuperString::UTF32::print(stream, (const Byte *) this->_container._reconstructed._data, startIndex,
                                      endIndex);
    }
    return isOk;
}

SuperString SuperString::ConcatenationSequence::trim() const {
    // TODO: General code, specify
    std::size_t startIndex = 0;
    std::size_t endIndex = this->length();
    Result<int, Error> result = this->codeUnitAt(startIndex);
    while(result.isOk() && SuperString::isWhiteSpace(result.ok())) {
        result = this->codeUnitAt(++startIndex);
    }
    result = this->codeUnitAt(endIndex - 1);
    while(result.isOk() && SuperString::isWhiteSpace(result.ok())) {
        result = this->codeUnitAt(--endIndex - 1);
    }
    return this->substring(startIndex, endIndex).ok();
}

SuperString SuperString::ConcatenationSequence::trimLeft() const {
    // TODO: General code, specify
    std::size_t startIndex = 0;
    Result<int, Error> result = this->codeUnitAt(startIndex);
    while(result.isOk() && SuperString::isWhiteSpace(result.ok())) {
        result = this->codeUnitAt(++startIndex);
    }
    return this->substring(startIndex, this->length()).ok();
}

SuperString SuperString::ConcatenationSequence::trimRight() const {
    // TODO: General code, specify
    std::size_t endIndex = this->length();
    Result<int, Error> result = this->codeUnitAt(endIndex - 1);
    while(result.isOk() && SuperString::isWhiteSpace(result.ok())) {
        result = this->codeUnitAt(--endIndex - 1);
    }
    return this->substring(0, endIndex).ok();
}

std::size_t SuperString::ConcatenationSequence::keepingCost() const {
    switch(this->kind()) {
        case Kind::CONCATENATION:
            return sizeof(ConcatenationSequence) + this->_container._concatenation._left->keepingCost() +
                   this->_container._concatenation._right->keepingCost();
        case Kind::LEFTRECONSTRUCTED:
            return sizeof(ConcatenationSequence) + this->_container._leftReconstructed._leftLength * sizeof(int) +
                   this->_container._leftReconstructed._right->keepingCost();
        case Kind::RIGHTRECONSTRUCTED:
            return sizeof(ConcatenationSequence) + this->_container._rightReconstructed._left->keepingCost() +
                   this->_container._rightReconstructed._rightLength * sizeof(int);
        case Kind::RECONSTRUCTED:
            return sizeof(ConcatenationSequence) + this->_container._reconstructed._length * sizeof(int);
    }
}

std::size_t SuperString::ConcatenationSequence::reconstructionCost(const StringSequence *sequence) const {
    switch(this->kind()) {
        case Kind::CONCATENATION:
            if(sequence == this->_container._concatenation._left) {
                return sizeof(ConcatenationSequence) +
                       this->_container._concatenation._left->length() * sizeof(int);
            } else if(sequence == this->_container._concatenation._right) {
                return sizeof(ConcatenationSequence) +
                       this->_container._concatenation._right->length() * sizeof(int);
            }
            return 0;
        case Kind::LEFTRECONSTRUCTED:
            if(sequence == this->_container._leftReconstructed._right) {
                return sizeof(ConcatenationSequence) +
                       this->_container._leftReconstructed._right->length() * sizeof(int);
            } else {
                return 0;
            }
        case Kind::RIGHTRECONSTRUCTED:
            if(sequence == this->_container._rightReconstructed._left) {
                return sizeof(ConcatenationSequence) +
                       this->_container._rightReconstructed._left->length() * sizeof(int);
            } else {
                return 0;
            }
        case Kind::RECONSTRUCTED:
            return 0;
    }
}

void SuperString::ConcatenationSequence::reconstruct(const StringSequence *sequence) const {
    ConcatenationSequence *self = ((ConcatenationSequence *) ((std::size_t) this));
    if(self->kind() == Kind::CONCATENATION) {
        struct ConcatenationMetaInfo old = self->_container._concatenation;
        if(old._left == sequence) {
            struct LeftReconstructedMetaInfo nw;
            nw._right = old._right;
            nw._leftLength = old._left->length();
            nw._leftData = new int[nw._leftLength];
            for(std::size_t i = 0; i < nw._leftLength; i++) {
                nw._leftData[i] = old._left->codeUnitAt(i).ok();
            }
            old._left->removeReferencer(self);
            if(old._left->refCount() == 0 && old._left->freeingCost() < old._left->keepingCost()) {
                old._left->doDelete();
            }
            self->_kind = Kind::LEFTRECONSTRUCTED;
            self->_container._leftReconstructed = nw;
        } else if(old._right == sequence) {
            struct RightReconstructedMetaInfo nw;
            nw._left = old._right;
            nw._rightLength = old._right->length();
            nw._rightData = new int[nw._rightLength];
            for(std::size_t i = 0; i < nw._rightLength; i++) {
                nw._rightData[i] = old._right->codeUnitAt(i).ok();
            }
            old._right->removeReferencer(self);
            if(old._right->refCount() == 0 && old._right->freeingCost() < old._right->keepingCost()) {
                old._right->doDelete();
            }
            self->_kind = Kind::RIGHTRECONSTRUCTED;
            self->_container._rightReconstructed = nw;
        }
    } else if(self->kind() == Kind::LEFTRECONSTRUCTED) {
        struct LeftReconstructedMetaInfo old = self->_container._leftReconstructed;
        if(old._right == sequence) {
            struct ReconstructedMetaInfo nw;
            nw._length = old._leftLength + old._right->length();
            nw._data = new int[nw._length];
            for(std::size_t i = 0; i < nw._length; i++) {
                if(i < old._leftLength) {
                    nw._data[i] = old._leftData[i];
                } else {
                    nw._data[i] = old._right->codeUnitAt(i - old._leftLength).ok();
                }
            }
            old._right->removeReferencer(self);
            if(old._right->refCount() == 0 && old._right->freeingCost() < old._right->keepingCost()) {
                old._right->doDelete();
            }
            self->_kind = Kind::RECONSTRUCTED;
            self->_container._reconstructed = nw;
        }
    } else if(self->kind() == Kind::RIGHTRECONSTRUCTED) {
        struct RightReconstructedMetaInfo old = self->_container._rightReconstructed;
        if(old._left == sequence) {
            struct ReconstructedMetaInfo nw;
            nw._length = old._left->length() + old._rightLength;
            nw._data = new int[nw._length];
            std::size_t leftLength = old._left->length();
            for(std::size_t i = 0; i < nw._length; i++) {
                if(i < leftLength) {
                    nw._data[i] = old._left->codeUnitAt(i).ok();
                } else {
                    nw._data[i] = old._rightData[i - leftLength];
                }
            }
            old._left->removeReferencer(self);
            if(old._left->refCount() == 0 && old._left->freeingCost() < old._left->keepingCost()) {
                old._left->doDelete();
            }
            self->_kind = Kind::RECONSTRUCTED;
            self->_container._reconstructed = nw;
        }
    }
}

void SuperString::ConcatenationSequence::doDelete() const {
    ConcatenationSequence *self = ((ConcatenationSequence *) (std::size_t) this);
    if(!this->isToBeDeleted()) {
        *((char *) &self->_kind) = ((char) self->kind()) + 0b10000000; // Just a trick, we don't want any more variable
        delete self;
    }
}

bool SuperString::ConcatenationSequence::isToBeDeleted() const {
    return (((char) this->_kind) & 0b10000000) == 0b10000000;
}

//*-- MultipleSequence (internal)
SuperString::MultipleSequence::MultipleSequence(const StringSequence *sequence, std::size_t time) {
    this->_kind = Kind::MULTIPLE;
    this->_container._multiple._time = time;
    this->_container._multiple._sequence = sequence;
    this->_container._multiple._sequence->addReferencer(this);
}

SuperString::MultipleSequence::~MultipleSequence() {
    this->reconstructReferencers();
    switch(this->kind()) {
        case Kind::MULTIPLE:
            this->_container._multiple._sequence->removeReferencer(this);
            if(this->_container._multiple._sequence->refCount() == 0 &&
               this->_container._multiple._sequence->freeingCost() <
               this->_container._multiple._sequence->keepingCost()) {
                this->_container._multiple._sequence->doDelete();
            }
            break;
        case Kind::RECONSTRUCTED:
            delete this->_container._reconstructed._data;
            break;
    }
}

SuperString::MultipleSequence::Kind SuperString::MultipleSequence::kind() const {
    return (Kind) (((char) this->_kind) & 0b01111111);
}

std::size_t SuperString::MultipleSequence::length() const {
    switch(this->kind()) {
        case Kind::MULTIPLE:
            return this->_container._multiple._sequence->length() * this->_container._multiple._time;
        case Kind::RECONSTRUCTED:
            return this->_container._reconstructed._dataLength * this->_container._reconstructed._time;
    }
}

SuperString::Result<int, SuperString::Error> SuperString::MultipleSequence::codeUnitAt(std::size_t index) const {
    std::size_t length = this->length();
    if(index < length) {
        switch(this->kind()) {
            case Kind::MULTIPLE:
                return this->_container._multiple._sequence->codeUnitAt(
                        index % this->_container._multiple._sequence->length());
            case Kind::RECONSTRUCTED:
                return this->_container._reconstructed._data[index % this->_container._reconstructed._dataLength];
        }
    }
    return Result<int, Error>(Error::RangeError);
}

SuperString::Result<SuperString, SuperString::Error>
SuperString::MultipleSequence::substring(std::size_t startIndex,
                                         std::size_t endIndex) const {
    std::size_t length = this->length();
    if(length < startIndex || length < endIndex) {
        return Result<SuperString, Error>(Error::RangeError);
    }
    SubstringSequence *sequence = new SubstringSequence(this, startIndex, endIndex);
    return Result<SuperString, Error>(SuperString(sequence));
}

bool SuperString::MultipleSequence::print(std::ostream &stream) const {
    switch(this->kind()) {
        case Kind::MULTIPLE:
            for(std::size_t i = 0; i < this->_container._multiple._time; i++) {
                this->_container._multiple._sequence->print(stream);
            }
            break;
        case Kind::RECONSTRUCTED:
            for(std::size_t i = 0; i < this->_container._multiple._time; i++) {
                SuperString::UTF32::print(stream, (const Byte *) this->_container._reconstructed._data);
            }
            break;
    }
    return true;
}

bool SuperString::MultipleSequence::print(std::ostream &stream, std::size_t startIndex,
                                                       std::size_t endIndex) const {
    bool printing = false;
    std::size_t unitLength;
    switch(this->kind()) {
        case Kind::MULTIPLE:
            unitLength = this->_container._multiple._sequence->length();
            for(std::size_t i = 0; i < this->_container._multiple._time; i++) {
                std::size_t iterationStartIndex = i * unitLength;
                std::size_t iterationEndIndex = (i + 1) * unitLength;
                if(!printing) {
                    if(iterationStartIndex <= startIndex) {
                        if(endIndex < iterationEndIndex) {
                            this->_container._multiple._sequence->print(stream, startIndex - iterationStartIndex,
                                                                        endIndex - iterationStartIndex);
                            break;
                        } else {
                            printing = true;
                            this->_container._multiple._sequence->print(stream, startIndex - iterationStartIndex,
                                                                        unitLength);
                        }
                    }
                } else {
                    if(endIndex <= iterationEndIndex) {
                        this->_container._multiple._sequence->print(stream, 0, endIndex - iterationStartIndex);
                    } else {
                        this->_container._multiple._sequence->print(stream);
                    }
                }
            }
            break;
        case Kind::RECONSTRUCTED:
            unitLength = this->_container._reconstructed._dataLength;
            for(std::size_t i = 0; i < this->_container._reconstructed._time; i++) {
                std::size_t iterationStartIndex = i * unitLength;
                std::size_t iterationEndIndex = (i + 1) * unitLength;
                if(!printing) {
                    if(iterationStartIndex <= startIndex) {
                        if(endIndex < iterationEndIndex) {
                            SuperString::UTF32::print(stream, (const Byte *) this->_container._reconstructed._data,
                                                      startIndex - iterationStartIndex,
                                                      endIndex - iterationStartIndex);
                            break;
                        } else {
                            printing = true;
                            SuperString::UTF32::print(stream, (const Byte *) this->_container._reconstructed._data,
                                                      startIndex - iterationStartIndex,
                                                      unitLength);
                        }
                    }
                } else {
                    if(endIndex <= iterationEndIndex) {
                        SuperString::UTF32::print(stream, (const Byte *) this->_container._reconstructed._data,
                                                  0, endIndex - iterationStartIndex);
                    } else {
                        SuperString::UTF32::print(stream, (const Byte *) this->_container._reconstructed._data);
                    }
                }
            }
            break;
    }
    return true;
}

SuperString SuperString::MultipleSequence::trim() const {
    // TODO: General code, specify
    std::size_t startIndex = 0;
    std::size_t endIndex = this->length();
    Result<int, Error> result = this->codeUnitAt(startIndex);
    while(result.isOk() && SuperString::isWhiteSpace(result.ok())) {
        result = this->codeUnitAt(++startIndex);
    }
    result = this->codeUnitAt(endIndex - 1);
    while(result.isOk() && SuperString::isWhiteSpace(result.ok())) {
        result = this->codeUnitAt(--endIndex - 1);
    }
    return this->substring(startIndex, endIndex).ok();
}

SuperString SuperString::MultipleSequence::trimLeft() const {
    // TODO: General code, specify
    std::size_t startIndex = 0;
    Result<int, Error> result = this->codeUnitAt(startIndex);
    while(result.isOk() && SuperString::isWhiteSpace(result.ok())) {
        result = this->codeUnitAt(++startIndex);
    }
    return this->substring(startIndex, this->length()).ok();
}

SuperString SuperString::MultipleSequence::trimRight() const {
    // TODO: General code, specify
    std::size_t endIndex = this->length();
    Result<int, Error> result = this->codeUnitAt(endIndex - 1);
    while(result.isOk() && SuperString::isWhiteSpace(result.ok())) {
        result = this->codeUnitAt(--endIndex - 1);
    }
    return this->substring(0, endIndex).ok();
}

std::size_t SuperString::MultipleSequence::keepingCost() const {
    switch(this->kind()) {
        case Kind::MULTIPLE:
            return sizeof(MultipleSequence) + this->_container._multiple._sequence->keepingCost();
        case Kind::RECONSTRUCTED:
            return sizeof(MultipleSequence) + this->_container._reconstructed._dataLength * sizeof(int);
    }
    return 0;
}

std::size_t SuperString::MultipleSequence::reconstructionCost(const StringSequence *sequence) const {
    switch(this->kind()) {
        case Kind::MULTIPLE:
            return sizeof(SubstringSequence) +
                   this->_container._multiple._sequence->length() * this->_container._multiple._time * 2;
        case Kind::RECONSTRUCTED:
            return 0;
    }
}

void SuperString::MultipleSequence::reconstruct(const StringSequence *sequence) const {
    MultipleSequence *self = ((MultipleSequence *) ((std::size_t) this));
    if(self->kind() == Kind::MULTIPLE) {
        struct MultipleMetaInfo old = self->_container._multiple;
        if(sequence == old._sequence) {
            struct ReconstructedMetaInfo nw;
            nw._time = old._time;
            nw._dataLength = old._sequence->length();
            nw._data = new int[nw._dataLength];
            for(std::size_t i = 0; i < nw._dataLength; i++) {
                nw._data[i] = old._sequence->codeUnitAt(i).ok();
            }
            old._sequence->removeReferencer(self);
            if(old._sequence->refCount() == 0 && old._sequence->freeingCost() < old._sequence->keepingCost()) {
                old._sequence->doDelete();
            }
            self->_kind = Kind::RECONSTRUCTED;
            self->_container._reconstructed = nw;
        }
    }
}

void SuperString::MultipleSequence::doDelete() const {
    MultipleSequence *self = ((MultipleSequence *) (std::size_t) this);
    if(!self->isToBeDeleted()) {
        self->_kind = (Kind) (((char) self->kind()) + 0b10000000); // Just a trick, we don't want any more variable
        delete self;
    }
}

bool SuperString::MultipleSequence::isToBeDeleted() const {
    return (((char) this->_kind) & 0b10000000) == 0b10000000;
}

//*-- SuperString::ASCII
std::size_t SuperString::ASCII::length(const SuperString::Byte *bytes) {
    const Byte *pointer = bytes;
    while(*pointer != 0x00) {
        pointer++;
    }
    return pointer - bytes;
}

int SuperString::ASCII::codeUnitAt(const SuperString::Byte *bytes, std::size_t index) {
    return ((int) *(bytes + index));
}

void SuperString::ASCII::print(std::ostream &stream, const SuperString::Byte *bytes) {
    stream << (const char *) bytes;
}

void SuperString::ASCII::print(std::ostream &stream, const SuperString::Byte *bytes, std::size_t startIndex,
                               std::size_t endIndex) {
    stream.write(((char *) (bytes + startIndex)), endIndex - startIndex);
}

SuperString::Pair<std::size_t, std::size_t>
SuperString::ASCII::trim(const SuperString::Byte *bytes, std::size_t length) {
    std::size_t startIndex = 0;
    std::size_t endIndex = length;
    char c = *(bytes + startIndex);
    while(c != 0x00 && SuperString::isWhiteSpace(c)) {
        c = *(bytes + (++startIndex));
    }
    c = *(bytes + (endIndex - 1));
    while(endIndex > 0 && SuperString::isWhiteSpace(c)) {
        c = *(bytes + (--endIndex - 1));
    }
    return Pair<std::size_t, std::size_t>(startIndex, endIndex);
}

std::size_t SuperString::ASCII::trimLeft(const SuperString::Byte *bytes) {
    std::size_t startIndex = 0;
    char c = *(bytes + startIndex);
    while(c != 0x00 && SuperString::isWhiteSpace(c)) {
        c = *(bytes + ++startIndex);
    }
    return startIndex;
}

std::size_t SuperString::ASCII::trimRight(const SuperString::Byte *bytes, std::size_t length) {
    std::size_t endIndex = length;
    char c = *(bytes + (endIndex - 1));
    while(endIndex > 0 && SuperString::isWhiteSpace(c)) {
        c = *(bytes + (--endIndex - 1));
    }
    return endIndex;
}

// SuperString::UTF8
std::size_t SuperString::UTF8::length(const SuperString::Byte *bytes) {
    std::size_t length = 0;
    const Byte *pointer = bytes;
    while(*pointer != 0x00) {
        if((*pointer & 0xf8) == 0xf0) { pointer += 4; }
        else if((*pointer & 0xf0) == 0xe0) { pointer += 3; }
        else if((*pointer & 0xe0) == 0xc0) { pointer += 2; }
        else if((*pointer & 0x80) == 0x00) { pointer++; }
        else return 0;
        length++;
    }
    return length;
}

SuperString::Pair<std::size_t, std::size_t>
SuperString::UTF8::lengthAndMemoryLength(const SuperString::Byte *bytes) {
    std::size_t length = 0;
    const Byte *pointer = bytes;
    while(*pointer != 0x00) {
        if((*pointer & 0xf8) == 0xf0) { pointer += 4; }
        else if((*pointer & 0xf0) == 0xe0) { pointer += 3; }
        else if((*pointer & 0xe0) == 0xc0) { pointer += 2; }
        else if((*pointer & 0x80) == 0x00) { pointer++; }
        else return Pair<std::size_t, std::size_t>(0, 0); // handle error
        length++;
    }
    return Pair<std::size_t, std::size_t>(length, pointer - bytes + 1);
}

SuperString::Result<int, SuperString::Error>
SuperString::UTF8::codeUnitAt(const SuperString::Byte *bytes, std::size_t index) {
    std::size_t i = 0;
    const Byte *pointer = bytes;
    while(*pointer != 0x00) {
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
            codeUnit = codeUnit << 6 | (*pointer & 0x3f);
        }
        if(i == index) {
            return Result<int, SuperString::Error>(codeUnit);
        }
        pointer++;
        i++;
    }
    return Result<int, SuperString::Error>(Error::RangeError);
}

void SuperString::UTF8::print(std::ostream &stream, const SuperString::Byte *bytes) {
    stream << bytes;
}

void SuperString::UTF8::print(std::ostream &stream, const SuperString::Byte *bytes, std::size_t startIndex,
                              std::size_t endIndex) {
    Result<Pair<std::size_t, std::size_t>, Error> result = SuperString::UTF8::rangeIndexes(bytes, startIndex, endIndex);
    if(result.isOk()) {
        stream.write(((char *) (bytes + result.ok().first())), result.ok().second() - result.ok().first());
    }
}

SuperString::Result<SuperString::Pair<std::size_t, std::size_t>, SuperString::Error>
SuperString::UTF8::rangeIndexes(
        const SuperString::Byte *bytes, std::size_t startIndex, std::size_t endIndex) {
    std::size_t i = 0;
    std::size_t startOffset, endOffset;
    bool first = false, second = false;
    const Byte *pointer = bytes;
    while(*pointer != '\0') {
        if(!first) {
            if(i == startIndex) {
                first = true;
                startOffset = ((std::size_t) pointer) - ((std::size_t) bytes);
            }
        } else {
            if(i == endIndex) {
                second = true;
                endOffset = ((std::size_t) pointer) - ((std::size_t) bytes);
                break;
            }
        }
        if((*pointer & 0xf8) == 0xf0) { pointer += 4; }
        else if((*pointer & 0xf0) == 0xe0) { pointer += 3; }
        else if((*pointer & 0xe0) == 0xc0) { pointer += 2; }
        else if((*pointer & 0x80) == 0x00) { pointer++; }
        else return Result<Pair<std::size_t, std::size_t>, Error>(Error::InvalidByteSequence);
        i++;
    }
    if(!second) {
        if(i == endIndex) {
            second = true;
            endOffset = ((std::size_t) pointer) - ((std::size_t) bytes);
        }
    }
    if(first && second) {
        return Result<Pair<std::size_t, std::size_t>, Error>(Pair<std::size_t, std::size_t>(startOffset, endOffset));
    }
    return Result<Pair<std::size_t, std::size_t>, Error>(Error::RangeError);
}

SuperString::Pair<SuperString::Byte *, std::size_t> SuperString::UTF8::codeUnitToChar(int c) {
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
    Byte *bytes = new Byte[numBytes];
    std::size_t j = 0;
    while(j < numBytes) {
        switch(j) {
            case 0:
                bytes[j] = byte1;
                break;
            case 1:
                bytes[j] = byte2;
                break;
            case 2:
                bytes[j] = byte3;
                break;
            case 3:
                bytes[j] = byte4;
                break;
            default:;
        }
        j++;
    }
    return Pair<Byte *, std::size_t>(bytes, numBytes);
}

// SuperString::UTF16BE
std::size_t SuperString::UTF16BE::length(const SuperString::Byte *bytes) {
    const Byte *pointer = bytes;
    while(*pointer != 0x00 || *(pointer + 1) != 0x00) {
        pointer += 2;
    }
    return (pointer - bytes) / 2;
}

SuperString::Pair<std::size_t, std::size_t>
SuperString::UTF16BE::lengthAndMemoryLength(const SuperString::Byte *bytes) {
    std::size_t length = 0;
    const Byte *pointer = bytes;
    while(*pointer != 0x00 || *(pointer + 1) != 0x00) {
        if((*pointer & 0xfc) == 0xd8) { pointer += 4; }
        else { pointer += 2; }
        length++;
    }
    return Pair<std::size_t, std::size_t>(length, pointer - bytes + 2);
}

SuperString::Result<int, SuperString::Error>
SuperString::UTF16BE::codeUnitAt(const SuperString::Byte *bytes, std::size_t index) {
    std::size_t i = 0;
    const Byte *pointer = bytes;
    while(*pointer != 0x00 || *(pointer + 1) != 0x00) {
        int codeUnit = 0;
        if((*pointer & 0xfc) == 0xd8) {
            codeUnit = (*pointer & 0x03) << 18;
            codeUnit += *(pointer + 1) << 10;
            codeUnit += (*(pointer + 2) & 0x03) << 8;
            codeUnit += *(pointer + 3);
            pointer += 4;
        } else {
            codeUnit = (*pointer << 8) + (*(pointer + 1));
            pointer += 2;
        }
        if(i == index) {
            return Result<int, SuperString::Error>(codeUnit);
        }
        i++;
    }
    return Result<int, SuperString::Error>(Error::RangeError);
}

void SuperString::UTF16BE::print(std::ostream &stream, const SuperString::Byte *bytes, std::size_t length) {
    SuperString::UTF16BE::print(stream, bytes, 0, length);
}

void SuperString::UTF16BE::print(std::ostream &stream, const SuperString::Byte *bytes, std::size_t startIndex,
                                 std::size_t endIndex) {
    std::size_t i = 0;
    const Byte *pointer = bytes;
    while((*pointer != 0x00 || *(pointer + 1) != 0x00) && i < endIndex) {
        int codeUnit = 0;
        if((*pointer & 0xfc) == 0xd8) {
            codeUnit = (*pointer & 0x03) << 18;
            codeUnit += *(pointer + 1) << 10;
            codeUnit += (*(pointer + 2) & 0x03) << 8;
            codeUnit += *(pointer + 3);
            pointer += 4;
        } else {
            codeUnit = (*pointer << 8) + (*(pointer + 1));
            pointer += 2;
        }
        if(startIndex <= i) {
            Pair<Byte *, std::size_t> encoded = SuperString::UTF8::codeUnitToChar(codeUnit);
            stream.write((const char *) encoded.first(), encoded.second());
            delete encoded.first();
        }
        i++;
    }
}

// SuperString::UTF32
std::size_t SuperString::UTF32::length(const SuperString::Byte *bytes) {
    const Byte *pointer = bytes;
    while(*((int *) pointer) != 0x00) {
        pointer += 4;
    }
    return (pointer - bytes) / 4;
}

SuperString::Pair<std::size_t, std::size_t> SuperString::UTF32::lengthAndMemoryLength(
        const SuperString::Byte *bytes) {
    std::size_t length = 0;
    const Byte *pointer = bytes;
    while(*((int *) pointer) != 0x00) {
        pointer += 4;
        length++;
    }
    return Pair<std::size_t, std::size_t>(length, pointer - bytes + 4);
}

int SuperString::UTF32::codeUnitAt(const SuperString::Byte *bytes, std::size_t index) {
    return *(((int *) bytes) + index);
}

void SuperString::UTF32::print(std::ostream &stream, const SuperString::Byte *bytes) {
    const Byte *pointer = bytes;
    while(*((int *) pointer) != 0x00) {
        int codeUnit = *((int *) pointer);
        Pair<Byte *, std::size_t> encoded = SuperString::UTF8::codeUnitToChar(codeUnit);
        stream.write((const char *) encoded.first(), encoded.second());
        delete encoded.first();
        pointer += 4;
    }
}

void SuperString::UTF32::print(std::ostream &stream, const SuperString::Byte *bytes, std::size_t startIndex,
                               std::size_t endIndex) {
    for(std::size_t i = startIndex; i < endIndex; i++) {
        int codeUnit = *(((int *) bytes) + i);
        Pair<Byte *, std::size_t> encoded = SuperString::UTF8::codeUnitToChar(codeUnit);
        stream.write((const char *) encoded.first(), encoded.second());
        delete encoded.first();
    }
}

SuperString::Pair<std::size_t, std::size_t>
SuperString::UTF32::trim(const SuperString::Byte *bytes, std::size_t length) {
    std::size_t startIndex = 0;
    std::size_t endIndex = length;
    int c = *(((const int *) bytes) + startIndex);
    while(c != 0x00 && SuperString::isWhiteSpace(c)) {
        c = *(((const int *) bytes) + ++startIndex);
    }
    c = *(((const int *) bytes) + (endIndex - 1));
    while(endIndex > 0 && SuperString::isWhiteSpace(c)) {
        c = *(((const int *) bytes) + (--endIndex - 1));
    }
    return Pair<std::size_t, std::size_t>(startIndex, endIndex);
}

std::size_t SuperString::UTF32::trimLeft(const SuperString::Byte *bytes) {
    std::size_t startIndex = 0;
    int c = *(((const int *) bytes) + startIndex);
    while(c != 0x00 && SuperString::isWhiteSpace(c)) {
        c = *(((const int *) bytes) + ++startIndex);
    }
    return startIndex;
}

std::size_t SuperString::UTF32::trimRight(const SuperString::Byte *bytes, std::size_t length) {
    std::size_t endIndex = length;
    int c = *(((const int *) bytes) + (endIndex - 1));
    while(endIndex > 0 && SuperString::isWhiteSpace(c)) {
        c = *(((const int *) bytes) + (--endIndex - 1));
    }
    return endIndex;
}

//
std::ostream &operator<<(std::ostream &stream, const SuperString &string) {
    string.print(stream);
    return stream;
}

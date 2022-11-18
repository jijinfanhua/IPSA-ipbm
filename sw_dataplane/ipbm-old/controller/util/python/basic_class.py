
class Field:
    def __init__(self, field_name, field_length, field_internal_offset):
        self.field_name = field_name
        self.field_length = field_length
        self.field_internal_offset = field_internal_offset

class NextHeader:
    def __init__(self, header_tag, header_name):
        self.header_tag = header_tag
        self.header_name = header_name

class Header:
    def __init__(self, header_name, header_length, field_num):
        self.header_name = header_name
        self.header_length = header_length
        self.field_num = field_num
        self.next_header_type_internal_offset = 0
        self.next_header_type_length = 0
        self.fields = []
        self.next_headers = []
    def setNextHeaderTypeInternalOffsetAndLength(self, offset, length):
        self.next_header_type_internal_offset = offset
        self.next_header_type_length = length
    def addField(self, field):
        self.fields.append(field)
    def addNextHeader(self, next_header):
        self.next_headers.append(next_header)

class stack(object):
    def __init__(self):
        self.data = []

    def push(self, ele):
        self.data.append(ele)

    def pop(self):
        return self.data.pop()

    def top(self):
        return self.data[-1]
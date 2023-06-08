# ets_utils
The ets_utils component provides four modules: js_api_module, js_util_module, js_sys_module, and js_concurrent_module. The following provides an introduction to these modules and describes their directory structures, APIs, and involved repositories.

# Modules
## js_api_module
### Introduction

The js_api_module provides the following classes:<br>**URL**: provides APIs to parse, construct, standardize, and encode URLs. The constructor of this class creates a **URL** object, where you can call APIs to parse and change the URL. **URLSearchParams**: provides APIs to handle URL query strings.

**URI**: provides APIs to parse URI strings.

**ConvertXml**: provides APIs to convert XML text into JavaScript objects.

**XmlSerializer**: provides APIs to generate an XML file. The constructor of this class creates an **XmlSerializer** object, where you can call the APIs to generate an XML file. **XmlPullParser**: provides APIs to parse an XML file. The constructor of this class creates an **XmlPullParser** object, where you can call the APIs to parse an XML file.

### Directory Structure

```
commonlibrary/ets_utils/js_api_module/
├── Class:URL                                                                     # URL class
│   ├── new URL(input[, base])                                                    # Used to create a URL object
│   ├── hash                                                                      # hash attribute
│   ├── host                                                                      # host attribute
│   ├── hostname                                                                  # hostname attribute
│   ├── href                                                                      # href attribute
│   ├── origin                                                                    # origin attribute
│   ├── password                                                                  # password attribute
│   ├── pathname                                                                  # pathname attribute
│   ├── port                                                                      # port attribute
│   ├── protocol                                                                  # protocol attribute
│   ├── search                                                                    # search attribute
│   ├── searchParams                                                              # searchParams attribute
│   ├── username                                                                  # username attribute
│   ├── toString()                                                                # toString method
│   └── toJSON()                                                                  # toJSON method
├── Class: URLSearchParams                                                        # URLSearchParams class
│   ├── new URLSearchParams()                                                     # Used to create a URLSearchParams object
│   ├── new URLSearchParams(string)                                               # Used to create a URLSearchParams object
│   ├── new URLSearchParams(obj)                                                  # Used to create a URLSearchParams object
│   ├── new URLSearchParams(iterable)                                             # Used to create a URLSearchParams object
│   ├── append(name, value)                                                       # append method
│   ├── delete(name)                                                              # delete method
│   ├── entries()                                                                 # entries method
│   ├── forEach(fn[, thisArg])                                                    # forEach method
│   ├── get(name)                                                                 # get method
│   ├── getAll(name)                                                              # getAll method
│   ├── has(name)                                                                 # has method
│   ├── keys()                                                                    # keys method
│   ├── set(name, value)                                                          # set method
│   ├── sort()                                                                    # sort method
│   ├── toString()                                                                # toString method
│   ├── values()                                                                  # values method
│   └── urlSearchParams[Symbol.iterator]()                                        # Used to create a URLSearchParams object
├── Class:URI                                                                     # URI class
│   ├── URI(str: string)                                                          # Used to create a URI object
│   ├── scheme                                                                    # scheme attribute
│   ├── authority                                                                 # authority attribute
│   ├── ssp                                                                       # ssp attribute
│   ├── userinfo                                                                  # userinfo attribute
│   ├── host                                                                      # host attribute
│   ├── port                                                                      # port attribute
│   ├── query                                                                     # query attribute
│   ├── fragment                                                                  # fragment attribute
│   ├── path                                                                      # path attribute
│   ├── equals(ob: Object)                                                        # equals attribute
│   ├── normalize()                                                               # normalize attribute
│   ├── checkIsAbsolute()                                                         # checkIsAbsolute attribute
│   ├── normalize()                                                               # normalize method
│   └── toString()                                                                # toString method
├── Class:ConvertXml                                                              # ConvertXml class
│   ├── ConvertXml()                                                              # Used to create a ConvertXml object
│   └── convert(xml: string, options: Object)                                     # convert method
├── Class:XmlSerializer                                                           # XmlSerializer class
│   ├── new XmlSerializer(buffer: ArrayBuffer | DataView, encoding?: string)      # Used to create an XmlSerializer object
│   ├── setAttributes(name: string, value: string)                                # setAttributes method
│   ├── addEmptyElement(name: string)                                             # addEmptyElement method
│   ├── setDeclaration()                                                          # setDeclaration method
│   ├── startElement(name: string)                                                # startElement method
│   ├── endElement()                                                              # endElement method
│   ├── setNamespace(prefix: string, namespace: string)                           # setNamespace method
│   ├── setComment(text: string)                                                  # setComment method
│   ├── setCData(text: string)                                                    # setCData method
│   ├── setText(text: string)                                                     # setText method
│   └── setDocType(text: string)                                                  # setDocType method
└── Class: XmlPullParser                                                          # XmlPullParser class
    ├── new (buffer: ArrayBuffer | DataView, encoding?: string)                   # Used to create an XmlPullParser object
    └── parse(option: ParseOptions)                                               # parse method
```

### Usage

#### Available APIs


| API| Description|
| -------- | -------- |
| URL(url: string,base?:string \| URL) | Creates a **URL** object that references a URL specified using an absolute URL string, a relative URL string, and a base URL string.|
| tostring():string | Converts the parsed URL into a string. Its function is the same as that of read-only **URL.href**.|
| toJSON():string | Converts the parsed URL into a JSON string.|
| new URLSearchParams() | No-argument constructor that creates a **URLSearchParams** object. The question mark (?) at the beginning of the query string will be ignored.|
| new URLSearchParams(string) | Constructor that creates a **URLSearchParams** object with the string type specified. The question mark (?) at the beginning of the query string will be ignored.|
| new URLSearchParams(obj) | Constructor that creates a **URLSearchParams** object with the object type specified. The question mark (?) at the beginning of the query string will be ignored.|
| new URLSearchParams(iterable) | Constructor that creates a **URLSearchParams** object with the iterable type specified. The question mark (?) at the beginning of the query string will be ignored.|
| has(name: string): boolean | Checks whether a key in this **URLSearchParams** object has a value. It returns **true** if the key has a value and returns **false** otherwise.|
| set(name: string, value string): void |  Sets the value for a key in this **URLSearchParams** object. If key-value pairs matching the specified key exist, the value of the first occurrence of the key-value pair will be changed to the specified value and other key-value pairs will be deleted. Otherwise, the key-value pair will be appended to the query string.|
| sort(): void | Sorts all key-value pairs in this **URLSearchParams** object based on the Unicode code points of the keys and returns **undefined**. |
| toString(): string | Obtains strings that can be used for URL query based on this **URLSearchParams** object.|
| keys(): iterableIterator\<string> | Obtains an iterator that contains the keys of all the key-value pairs in this **URLSearchParams** object.|
| values(): iterableIterator\<string> | Obtains an iterator that contains the values of all the key-value pairs in this **URLSearchParams** object.|
| append(name: string, value: string): void | Appends a key-value pair into this **URLSearchParams** object.|
| delete(name: string): void | Deletes key-value pairs of the specified key from this **URLSearchParams** object.|
| get(name: string): string | Obtains the value of the first key-value pair with the specified key in this **URLSearchParams** object.|
| getAll(name: string): string[] | Obtains all the key-value pairs with the specified key in this **URLSearchParams** object.|
| entries(): iterableIterator<[string, string]> | Obtains an iterator that contains all the key-value pairs of this **URLSearchParams** object.|
| forEach(): void | Traverses the key-value pairs in this **URLSearchParams** object by using a callback.|
| urlSearchParams\[Symbol.iterator]() | Obtains an ES6 iterator. Each item of the iterator is a JavaScript array, and each array contains a key and value.|
| URI​(str: string) | Constructs a URI by parsing the given input parameter according to the syntax specified in Appendix A of RFC 2396.|
| scheme​ | Obtains the scheme component of this URI; returns **null** if **scheme** is not defined.|
| authority​ | Obtains the decoded authority component of this URI; returns **null** if **authority** is not defined.|
| ssp​ |  Obtains the decoded scheme-specific component of this URI.|
| userinfo​ | Obtains the decoded userinfo component of this URI. The return value contains password and username.|
| host​ | Obtains the host component of this URI; returns **null** if **host** is not defined.|
| port​ | Obtains the port component of this URI; returns **-1** if **port** is not defined. The return value is a non-negative integer.|
| query​ | Obtains the query component of this URI; returns **null** if **query** is not defined.|
| fragment​ | Obtains the decoded fragment component of this URI; returns **null** if **fragment** is not defined.|
| path​ | Obtains the decoded path component of this URI; returns **null** if **path** is not defined.|
| equals(ob: Object) | Checks whether this URI is the same as another URI object. It immediately returns **false** if the given object is not a URI.|
| normalize​() | Normalizes the path of this URI. It constructs a new URI object if the path of the URI is not standard.|
| checkIsAbsolute​() | Checks whether this URI is an absolute URI (whether the scheme component is defined). It returns **true** if the URI is an absolute URI; otherwise, it returns **false**.|
| ConvertXml() | A no-argument constructor that creates a **ConvertXml** object.  |
| convert(xml: string, options: Object)  | Converts an XML text into a JavaScript object.|
| XmlSerializer(buffer: ArrayBuffer \| DataView, encoding?: string) | Creates and returns an **XmlSerializer** object. The **XmlSerializer** object passes two parameters. The first parameter is the memory of the **ArrayBuffer** or **DataView** type, and the second parameter is the file format (UTF-8 by default).|
| setAttributes(name: string, value: string): void | Sets attributes for the XML file.|
| addEmptyElement(name: string): void | Adds an empty element.|
| setDeclaration(): void | Sets a declaration for the XML file. Example: <? xml version="1.0"encoding="utf-8">|
| startElement(name: string): void | Writes the start tag based on the given element name.|
| endElement(): void | Writes the end tag of the element.|
| setNamespace(prefix: string, namespace: string): void | Sets the namespace for an element tag.|
| setComment(text: string): void | Sets the comment.|
| setCData(text: string): void | Sets the **CData** attribute.|
| setText(text: string): void | Sets the **Text** attribute.|
| setDocType(text: string): void | Sets the **DocType** attribute.|
| XmlPullParser(buffer: ArrayBuffer \| DataView, encoding?: string) | Creates and returns an **XmlPullParser** object. The **XmlPullParser** object passes two parameters. The first parameter is the memory of the **ArrayBuffer** or **DataView** type, and the second parameter is the file format (UTF-8 by default).|
| parse(option: ParseOptions): void | Parses XML information. **ParseOptions** contains five optional parameters: {supportDoctype?: boolean ignoreNameSpace?: boolean tagValueCallbackFunction?: (name: string, value: string) => boolean attributeValueCallbackFunction?: (name: string, value: string) => boolean) tokenValueCallbackFunction?: (eventType: EventType, value: ParseInfo) => boolean }. For **tokenValueCallbackFunction**, the first input parameter indicates the event type, and the second input parameter indicates the **info** interface that contains getters such as **getColumnNumber** and **getDepth**. You can use methods such as **info.getDepth()** to obtain information during the parsing.|
#### How to Use

Use the APIs as follows:


1. new URL(url: string,base?:string|URL)
```
let b = new URL('https://developer.mozilla.org'); // => 'https://developer.mozilla.org/'

let a = new URL( 'sca/./path/path/../scasa/text', 'http://www.example.com');
// => 'http://www.example.com/sca/path/scasa/text'
```
2. tostring():string
```
const url = new URL('http://10.0xFF.O400.235:8080/directory/file?query#fragment');
url.toString() // => 'http://10.0xff.o400.235:8080/directory/file?query#fragment'

const url = new URL("http://[FEDC:BA98:7654:3210:FEDC:BA98:7654:3210]:80/index.html");
url.toString() // => 'http://[fedc:ba98:7654:3210:fedc:ba98:7654:3210]/index.html'

const url = new URL("http://username:password@host:8080/directory/file?query#fragment");
url.toString() // => 'http://username:password@host:8080/directory/file?query#fragment'
```
3. toJSON():string
```
const url = new URL("https://developer.mozilla.org/en-US/docs/Web/API/URL/toString");
url.toJSON(); // =>  'https://developer.mozilla.org/en-US/docs/Web/API/URL/toString'
```
4. new URLSearchParams()
```
let params = new URLSearchParams('foo=1&bar=2');
```
5. new URLSearchParams(string)
```
params = new URLSearchParams('user=abc&query=xyz');
console.log(params.get('user'));
// Prints 'abc'
```
6. new URLSearchParams(obj)
```
const params = new URLSearchParams({
    user: 'abc',
    query: ['first', 'second']
});
console.log(params.getAll('query'));
// Prints [ 'first,second' ]
```
7. new URLSearchParams(iterable)
```
let params;

// Using an array
params = new URLSearchParams([
    ['user', 'abc'],
    ['query', 'first'],
    ['query', 'second'],
]);
console.log(params.toString());
// Prints 'user = abc & query = first&query = second'
```
8. has(name: string): boolean
```
console.log(params.has('bar')); // =>true
```
9. set(name: string, value string): void
```
params.set('baz', 3);
```
10. sort(): void
```
params .sort();
```
11. toString(): string
```
console.log(params .toString()); // =>bar=2&baz=3&foo=1'
```
12. keys(): iterableIterator\<string>
```
for(var key of params.keys()) {
  console.log(key);
} // =>bar  baz  foo
```
13. values(): iterableIterator\<string>
```
for(var value of params.values()) {
  console.log(value);
} // =>2  3  1
```
14. append(name: string, value: string): void
```
params.append('foo', 3); // =>bar=2&baz=3&foo=1&foo=3
```
15. delete(name: string): void
```
params.delete('baz'); // => bar=2&foo=1&foo=3
```
16. get(name: string): string
```
params.get('foo'); // => 1
```
17. getAll(name: string): string[]
```
params.getAll('foo'); // =>[ '1', '3' ]
```
18. entries(): iterableIterator<[string, string]>
```
for(var pair of searchParams.entries()) {
   console.log(pair[0]+ ', '+ pair[1]);
} // => bar, 2   foo, 1  foo, 3
```
19. forEach(): void
```
url.searchParams.forEach((value, name, searchParams) => {
  console.log(name, value, url.searchParams === searchParams);
});
// => foo 1 true
// => bar 2 true
```
20. urlSearchParams[Symbol.iterator]()
```
const params = new URLSearchParams('foo=bar&xyz=baz');
for (const [name, value] of params) {
    console.log(name, value);
}
// Prints:
// foo bar
// xyz bar
```


21. URI(str: string)
```
let gaogao = new Uri.URI('http://gg:gaogao@www.baidu.com:99/path/path?query#fagment');
```
22. scheme
```
let gaogao = new Uri.URI('http://gg:gaogao@www.baidu.com:99/path/path?query#fagment');
gaogao.scheme        // => "http";
```
23. authority
```
let gaogao = new Uri.URI('http://gg:gaogao@www.baidu.com:99/path/path?query#fagment');
gaogao.authority     // => "gg:gaogao@www.baidu.com:99";
```
24. ssp
```
let gaogao = new Uri.URI('http://gg:gaogao@www.baidu.com:99/path/path?query#fagment');
gaogao.ssp "         // => gg:gaogao@www.baidu.com:99/path/path?query";
```
25. userinfo
```
let gaogao = new Uri.URI('http://gg:gaogao@www.baidu.com:99/path/path?query#fagment');
gaogao.userinfo      // => "gg:gaogao";
```
26. host
```
let gaogao = new Uri.URI('http://gg:gaogao@www.baidu.com:99/path/path?query#fagment');
gaogao.host          // => "www.baidu.com";
```
27. port
```
let gaogao = new Uri.URI('http://gg:gaogao@www.baidu.com:99/path/path?query#fagment');
gaogao.port          // => "99";
```
28. query
```
let gaogao = new Uri.URI('http://gg:gaogao@www.baidu.com:99/path/path?query#fagment');
gaogao.query         // => "query";
```
29. fragment
```
let gaogao = new Uri.URI('http://gg:gaogao@www.baidu.com:99/path/path?query#fagment');
gaogao.fragment      // => "fagment";
```
30. path
```
let gaogao = new Uri.URI('http://gg:gaogao@www.baidu.com:99/path/path?query#fagment');
gaogao.path          // => "/path/path";
```
31. equals(ob: Object)
```
let gaogao = new Uri.URI('http://gg:gaogao@[1:0:0:1:2:1:2:1]:99/path1?query#fagment');
let gaogao1 = gaogao;
let res = gaogao.equals(gaogao1);
console.log(res);      // => true;
```
32. normalize()
```
let gaogao = new Uri.URI('http://gg:gaogao@[1:0:0:1:2:1:2:1]:99/path/66./../././mm/.././path1?query#fagment');
let res = gaogao.normalize();
console.log(res.path);        // => "/path/path1"
console.log(res.toString());  // => "http://gg:gaogao@[1:0:0:1:2:1:2:1]:99/path/path1?query#fagment"
```
33. checkIsAbsolute()
```
let gaogao = new Uri.URI('f/tp://username:password@www.baidu.com:88/path?query#fagment');
let res = gaogao.checkIsAbsolute();
console.log(res);              //=> false;
```
34. toString()
```
let gaogao = new Uri.URI('http://gg:gaogao@[1:0:0:1:2:1:2:1]:99/../../path/.././../aa/bb/cc?query#fagment');
let res = gaogao.toString();
console.log(res.toString());     // => 'http://gg:gaogao@[1:0:0:1:2:1:2:1]:99/../../path/.././../aa/bb/cc?query#fagment';
```


35. ConvertXml()
```
var convertml = new convertXml.ConvertXml();
```
36. convert(xml: string, options: Object)
```
var result = convertml.convert(xml, {compact: false, spaces: 4});
```
37. new XmlSerializer(buffer: ArrayBuffer | DataView, encoding?: string)
```

var arrayBuffer = new ArrayBuffer(1024);
var bufView = new DataView(arrayBuffer);
var thatSer = new xml.XmlSerializer(bufView);
```
38. setDeclaration(): void
```
var thatSer = new xml.XmlSerializer(bufView);
thatSer.setDeclaration() // => <?xml version="1.0" encoding="utf-8"?>;
```
39. setComment(text: string):void
```
var thatSer = new xml.XmlSerializer(bufView);
thatSer.setCommnet("Hello, World!"); // => <!--Hello, World!-->;
```
40. setCData(text: string) :void
```
var thatSer = new xml.XmlSerializer(bufView);
thatSer.setDocType('root SYSTEM "http://www.test.org/test.dtd"'); // => <![CDATA[root SYSTEM \"http://www.test.org/test.dtd\"]]>
```
41. setDocType(text: string):void
```
var thatSer = new xml.XmlSerializer(bufView);
thatSer.setDocType("foo"); // => <!DOCTYPE foo>
```
42. setNamespace(prefix: string, namespace: string): void
43. startElement(name: string): void
44. setAttributes(name: string, value: string): void
45. endElement(): void
46. setText(text: string): void
```
var thatSer = new xml.XmlSerializer(bufView);
thatSer.setNamespace("h", "http://www.w3.org/TR/html4/");
thatSer.startElement("table");
thatSer.setAttributes("importance", "high");
thatSer.setText("Happy");
endElement(); // => <h:table importance="high" xmlns:h="http://www.w3.org/TR/html4/">Happy</h:table>
```
47. addEmptyElement(name: string): void
```
var thatSer = new xml.XmlSerializer(bufView);
thatSer.addEmptyElement("b"); // => <b/>
```
48. new (buffer: ArrayBuffer | DataView, encoding?: string)
```
var strXml =
            '<?xml version="1.0" encoding="utf-8"?>' +
            '<note importance="high" logged="true">' +
            '    <title>Happy</title>' +
            '</note>';
var arrayBuffer = new ArrayBuffer(strXml.length*2);
var bufView = new Uint8Array(arrayBuffer);
var strLen = strXml.length;
for (var i = 0; i < strLen; ++i) {
    bufView[i] = strXml.charCodeAt(i); // Set the ArrayBuffer mode.
}
var that = new xml.XmlPullParser(arrayBuffer);

```
49. parse(option: ParseOptions): void
```
var strXml =
            '<?xml version="1.0" encoding="utf-8"?>' +
            '<note importance="high" logged="true">' +
            '    <title>Happy</title>' +
            '</note>';
var arrayBuffer = new ArrayBuffer(strXml.length*2);
var bufView = new Uint8Array(arrayBuffer);
var strLen = strXml.length;
for (var i = 0; i < strLen; ++i) {
    bufView[i] = strXml.charCodeAt(i);
}
var that = new xml.XmlPullParser(arrayBuffer);
var arrTag = {};
arrTag[0] = '132';
var i = 1;
function func(key, value){
    arrTag[i] = 'key:'+key+' value:'+ value.getDepth();
    i++;
    return true;
}
var options = {supportDoctype:true, ignoreNameSpace:true, tokenValueCallbackFunction:func}
that.parse(options);
```

## js_util_module
### Introduction

The js_util_module provides the following classes:<br> **TextEncoder**: provides APIs to encode a string into a UTF-8 byte stream. **TextDecoder**: provides APIs to decode a byte stream into a string. **HelpFunction**: provides APIs to perform the callback and promise processing on functions, compile and output error codes, and format class strings. **encode**: provides APIs to use the Base64 encoding scheme to encode all bytes in a Unit8Array typed array into a new Unit8Array typed array or encode a byte array into a string. **decode**: provides APIs to use the Base64 encoding scheme to decode a Base64-encoded string or a Unit8Array typed array into a new Unit8Array typed array. **RationalNumber**: provides APIs to compare rational numbers and obtain numerators and denominators. **LruBuffer**: provides APIs to discard the least recently used data to make rooms for new elements when the buffer is full. The Least Recently Used (LRU) algorithm believes that the recently used data may be accessed again in the near future and the least accessed data is the least valuable data and should be removed from the buffer. **Scope**: provides APIs to define the valid range of a field. The constructor of this class creates comparable objects with lower and upper limits.
### Directory Structure

```
commonlibrary/ets_utils/js_util_module/
├── Class:TextEncoder                   # TextEncoder class
│   ├──  new TextEncoder()              # Used to create a TextEncoder object
│   ├──  encode()                       # encode method
│   ├──  encoding                       # encoding attribute
│   └──  encodeInto()                   # encodeInto method
├── Class:TextDecoder                   # TextDecoder class
│   ├──  new TextDecoder()              # Used to create a TextDecoder object
│   ├──  decode()                       # decode method
|   ├──  decodeWithStream()             # decodeWithStream method
│   ├──  encoding                       # encoding attribute
│   ├──  fatal                          # fatal attribute
│   └──  ignoreBOM                      # ignoreBOM attribute
├── printf()                            # printf method
├── getErrorString()                    # getErrorString method
├── callbackWrapper()                   # callbackWrapper method
├── promiseWrapper()                    # promiseWrapper method
├── Class:Base64                        # Base64 class
│   ├──  new Base64()                   # Used to create a Base64 object
│   ├──  encodeSync()                   # encodeSync method
│   ├──  encodeToStringSync()           # encodeToStringSync method
│   ├──  decodeSync()                   # decodeSync method
│   ├──  encode()                       # encode method
│   ├──  encodeToString()               # encodeToString method
│   └──  decode()                       # decode method
├── Class:RationalNumber                # RationalNumber class
│   ├──  new RationalNumber()           # Used to create a RationalNumber object
│   ├──  createRationalFromString()     # createRationalFromString method
│   ├──  compareTo()                    # compareTo method
│   ├──  equals()                       # equals method
│   ├──  valueOf()                      # valueOf method
│   ├──  getCommonDivisor()             # getCommonDivisor method
│   ├──  getDenominator()               # getDenominator method
│   ├──  getNumerator()                 # getNumerator method
│   ├──  isFinite()                     # isFinite method
│   ├──  isNaN()                        # isNaN method
│   ├──  isZero()                       # isZero method
│   └──  toString()                     # toString method
├── Class:LruBuffer                     # LruBuffer class
│   ├──  new LruBuffer()                # Used to create a LruBuffer object
│   ├──  updateCapacity()               # updateCapacity method
│   ├──  toString()                     # toString method
│   ├──  values()                       # values method
│   ├──  length                         # length attribute
│   ├──  getCapacity()                  # getCapacity method
│   ├──  clear()                        # clear method
│   ├──  getCreateCount()               # getCreateCount method
│   ├──  getMissCount()                 # getMissCount method
│   ├──  getRemovalCount()              # getRemovalCount method
│   ├──  getMatchCount()                # getMatchCount method
│   ├──  getPutCount()                  # getPutCount method
│   ├──  isEmpty()                      # isEmpty method
│   ├──  get()                          # get method
│   ├──  put()                          # put method
│   ├──  keys()                         # keys method
│   ├──  remove()                       # remove method
│   ├──  afterRemoval()                 # afterRemoval method
│   ├──  contains()                     # contains method
│   ├──  createDefault()                # createDefault method
│   ├──  entries()                      # entries method
│   └──  [Symbol.iterator]()            # Symboliterator method
|—— Class:Scope                         # Scope class
|   ├── constructor()                   # Used to create a Scope object
|   ├── toString()                      # toString method
|   ├── intersect()                     # intersect method
|   ├── intersect()                     # intersect method
|   ├── getUpper()                      # getUpper method
|   ├── getLower()                      # getLower method
|   ├── expand()                        # expand method
|   ├── expand()                        # expand method
|   ├── expand()                        # expand method
|   ├── contains()                      # contains method
|   ├── contains()                      # contains method
|   └── clamp()                         # clamp method
└── Class:Types                         # Types class
    ├── isAnyArrayBuffer()              # isAnyArrayBuffer method
    ├── isArrayBufferView()             # isArrayBufferView method
    ├── isArgumentsObject()             # isArgumentsObject method
    ├── isArrayBuffer()                 # isArrayBuffer method
    ├── isAsyncFunction()               # isAsyncFunction method
    ├── isBigInt64Array()               # isBigInt64Array method
    ├── isBigUint64Array()              # isBigUint64Array method
    ├── isBooleanObject()               # isBooleanObject method
    ├── isBoxedPrimitive()              # isBoxedPrimitive method
    ├── isDataView()                    # isDataView method
    ├── isDate()                        # isDate method
    ├── isExternal()                    # isExternal method
    ├── isFloat32Array()                # isFloat32Array method
    ├── isFloat64Array()                # isFloat64Array method
    ├── isGeneratorFunction()           # isGeneratorFunction method
    ├── isGeneratorObject()             # isGeneratorObject method
    ├── isInt8Array()                   # isInt8Array method
    ├── isInt16Array()                  # isInt16Array method
    ├── isInt32Array()                  # isInt32Array method
    ├── isMap()                         # isMap method
    ├── isMapIterator()                 # isMapIterator method
    ├── isModuleNamespaceObject()       # isModuleNamespaceObject method
    ├── isNativeError()                 # isNativeError method
    ├── isNumberObject()                # isNumberObject method
    ├── isPromise()                     # isPromise method
    ├── isProxy()                       # isProxy method
    ├── isRegExp()                      # isRegExp method
    ├── isSet()                         # isSet method
    ├── isSetIterator()                 # isSetIterator method
    ├── isSharedArrayBuffer()           # isSharedArrayBuffer method
    ├── isStringObject()                # isStringObject method
    ├── isSymbolObject()                # isSymbolObject method
    ├── isTypedArray()                  # isTypedArray method
    ├── isUint8Array()                  # isUint8Array method
    ├── isUint8ClampedArray()           # isUint8ClampedArray method
    ├── isUint16Array()                 # isUint16Array method
    ├── isUint32Array()                 # isUint32Array method
    ├── isWeakMap()                     # isWeakMap method
    └── isWeakSet()                     # isWeakSet method
```
### Usage

#### Available APIs


| API| Description|
| -------- | -------- |
| constructor(encoding? : string) | A constructor used to create a **TextDecoder** object. **encoding** indicates the encoding format. The default value is **UTF-8**, and the values **gb18030**, **gbk**, and **gb2312** are supported.|
| readonly encoding : string | Obtains the encoding format. Only **UTF-8** is supported.|
| encode(input : string) : Uint8Array | Encodes a string into a Uint8Array typed array.|
| encodeInto(input : string, dest : Uint8Array) : {read : number, written : number} | Encodes a string into a Unit8Array typed array, and stores the array. **dest** indicates the storage location, **read** indicates the number of encoded characters, and **written** indicates the number of bytes occupied by the encoded characters.|
| constructor(encoding? : string, options? : {fatal? : boolean, ignoreBOM? : boolean}) | A constructor used to create a **TextDecoder** object. **encoding** indicates the decoding format. **options** indicates certain attributes, where **fatal** specifies whether to throw an exception, and **ignoreBOM** specifies whether to ignore the BOM flag.|
| readonly encoding : string | Obtains the decoding format.|
| readonly fatal : boolean | Obtains the setting of whether to throw an exception.|
| readonly ignoreBOM : boolean | Obtains the setting of whether to ignore the BOM flag.|
| decode(input : Uint8Array, options?: { stream?: false }) : string | Decodes a Unit8Array typed array into a string. **input** indicates the data to decode, and **options** specifies whether additional data will be followed, with the default value **false**.|
| decodeWithStream(input : Uint8Array, options?: { stream?: false }) : string | Decodes a Unit8Array typed array into a string. **input** indicates the data to decode, and **options** specifies whether additional data will be followed, with the default value **false**.|
| encodeSync(src: Uint8Array): Uint8Array; | Uses the Base64 encoding scheme to synchronously encode all bytes in a Unit8Array typed array into a new Unit8Array typed array.|
| encodeToStringSync(src: Uint8Array): string; | Uses the Base64 encoding scheme to synchronously encode a Unit8Array typed array into a string.|
| decodeSync(src: Uint8Array \| string): Uint8Array; | Uses the Base64 encoding scheme to synchronously decode a Base64-encoded string or a Unit8Array typed array into a new Unit8Array typed array.|
| encode(src: Uint8Array): Promise\<Uint8Array\>; | Uses the Base64 encoding scheme to asynchronously encode all bytes in a Unit8Array typed array into a new Unit8Array typed array.|
| encodeToString(src: Uint8Array): Promise\<string\>; | Uses the Base64 encoding scheme to asynchronously encode a Unit8Array typed array into a string. It uses a promise to return the result.|
| decode(src: Uint8Array \| string): Promise\<Uint8Array\>; | Uses the Base64 encoding scheme to asynchronously decode a Base64-encoded string or a Unit8Array typed array into a new Unit8Array typed array.|
| static createRationalFromString(rationalString: string): RationalNumber | Creates a **RationalNumber** object based on the given string.|
| compareTo(another: RationalNumber): number | Compares this **RationalNumber** object with the given object.|
| equals(obj: object): number | Checks whether this **RationalNumber** object equals the given object.|
| valueOf(): number | Rounds the value of this **RationalNumber** object as an integer or a floating-point number.|
| static getCommonDivisor(number1: number, number2: number,): number | Obtains the greatest common divisor of two specified integers.|
| getDenominator(): number | Obtains the denominator of this **RationalNumber** object.|
| getNumerator(): number | Obtains the numerator of this **RationalNumber** object.|
| isFinite(): boolean | Checks whether this **RationalNumber** object represents a finite value.|
| isNaN(): boolean | Checks whether this **RationalNumber** object is a Not a Number (NaN).|
| isZero(): boolean | Checks whether this **RationalNumber** object is **0**.|
| toString(): string | Obtains the string representation of this **RationalNumber** object.|
| constructor(capacity?: number) | Default constructor used to create an **LruBuffer** instance. The default capacity of the buffer is 64.|
| updateCapacity(newCapacity: number): void | Changes the **LruBuffer** capacity. If the new capacity is less than or equal to **0**, an exception will be thrown.|
| toString(): string | Obtains the string representation of this **LruBuffer** object. |
| values(): V[] | Obtains all values in this buffer, listed from the most to the least recently used.|
| length: number | Obtains the total number of values in this buffer.|
| getCapacity(): number | Obtains the capacity of this buffer.|
| clear(): void | Clears key-value pairs from this buffer. **afterRemoval()** will be called to perform subsequent operations.|
| getCreateCount(): number | Obtains the number of return values for **createDefault()**.|
| getMissCount(): number | Obtains the number of times that the queried values are mismatched.|
| getRemovalCount(): number | Obtains the number of removals from this buffer.|
| getMatchCount​(): number | Obtains the number of times that the queried values are matched.|
| getPutCount(): number | Obtains the number of additions to this buffer.|
| isEmpty(): boolean | Checks whether this buffer is empty. This API returns **true** if the buffer does not contain any value.|
| get(key: K) : V \| undefined | Obtains the value of the specified key. This API returns the value of the key if a match is found in the buffer; otherwise, it returns **undefined**.|
| put(key: K , value: V): V | Adds a key-value pair to the buffer and outputs the value associated with the key. This API returns the existing value if the key already exists; otherwise, it returns the value added. If the key or value is null, an exception will be thrown.|
| keys(): K[ ] | Obtains all keys in this buffer, listed from the most to the least recently used.|
| remove​(key: K): V \| undefined |  Removes the specified key and its value from this buffer.|
| afterRemoval(isEvict: boolean, key: K, value : V, newValue : V):void | Performs subsequent operations after a value is removed.|
| contains(key: K): boolean | Checks whether this buffer contains the specified key. This API returns **true** if the buffer contains the specified key.|
| createDefault(key: K): V | Creates a value if the value of the specified key is not available.|
| entries(): [K,V] | Obtains a new iterator object that contains all key-value pairs in this buffer. The key and value of each pair are objects.|
| \[Symbol.iterator\](): [K,V] | Obtains a two-dimensional array in key-value pairs.|
| constructor(lowerObj: ScopeType, upperObj : ScopeType) | A constructor used to create a **Scope** object with the specified upper and lower limits.|
| toString(): string | Obtains a string representation that contains this **Scope**.|
| intersect(range: Scope): Scope | Obtains the intersection of this **Scope** and the given **Scope**.|
| intersect(lowerObj: ScopeType, upperObj: ScopeType): Scope | Obtains the intersection of this **Scope** and the given lower and upper limits.|
| getUpper(): ScopeType | Obtains the upper limit of this **Scope**.|
| getLower(): ScopeType | Obtains the lower limit of this **Scope**.|
| expand(lowerObj: ScopeType, upperObj:  ScopeType): Scope | Obtains the union set of this **Scope** and the given lower and upper limits.|
| expand(range: Scope): Scope | Obtains the union set of this **Scope** and the given **Scope**.|
| expand(value: ScopeType): Scope | Obtains the union set of this **Scope** and the given value.|
| contains(value: ScopeType): boolean | Checks whether a value is within this **Scope**.|
| contains(range: Scope): boolean | Checks whether a range is within this **Scope**.|
| clamp(value: ScopeType): ScopeType | Limits a value to this **Scope**.|
| function printf(format: string, ...args: Object[]): string | Prints the input content in a formatted string. **format** can contain zero or more format specifiers.|
| function getErrorString(errno: number): string | Obtains detailed information about a system error code.|
| function callbackWrapper(original: Function): (err: Object, value: Object) => void | Wraps an asynchronous function (or a function that returns a promise) into an error-first callback, which means that **(err, value) => ...** is used as the last parameter of the callback. In the callback, the first parameter indicates the cause of the rejection (the value is **null** if the promise has been resolved), and the second parameter indicates the resolved value.|
| function promiseWrapper(original: (err: Object, value: Object) => void): Object | Wraps a function that follows the error-first callback paradigm into a promise.|
| isAnyArrayBuffer(value: Object): boolean | Checks whether the input value is of the **ArrayBuffer** or **SharedArrayBuffer** type.|
| isArrayBufferView(value: Object): boolean | Checks whether the input value is of the **ArrayBufferView** type, which is a helper type representing any of the following: napi_int8_array, napi_uint8_array, napi_uint8_clamped_array, napi_int16_array, napi_uint16_array, napi_int32_array, napi_uint32_array, napi_float32_array, napi_float64_array, and DataView.|
| isArgumentsObject(value: Object): boolean | Checks whether the input value is of the **arguments** type.|
| isArrayBuffer(value: Object): boolean | Checks whether the input value is of the **ArrayBuffer** type.|
| isAsyncFunction(value: Object): boolean | Checks whether the input value is an asynchronous function.|
| isBigInt64Array(value: Object): boolean | Checks whether the input value is of the **BigInt64Array** type.|
| isBigUint64Array(value: Object): boolean | Checks whether the input value is of the **BigUint64Array** type.|
| isBooleanObject(value: Object): boolean | Checks whether the input value is of the **Boolean** type.|
| isBoxedPrimitive(value: Object): boolean | Checks whether the input value is of the **Boolean**, **Number**, **String**, or **Symbol** type.|
| isDataView(value: Object): boolean | Checks whether the input value is of the **DataView** type.|
| isDate(value: Object): boolean | Checks whether the input value is of the **Date** type.|
| isExternal(value: Object): boolean | Checks whether the input value is of the **native external** type.|
| isFloat32Array(value: Object): boolean | Checks whether the input value is of the **Float32Array** type.|
| isFloat64Array(value: Object): boolean | Checks whether the input value is of the **Float64Array** type.|
| isGeneratorFunction(value: Object): boolean | Checks whether the input value is a generator function.|
| isGeneratorObject(value: Object): boolean | Checks whether the input value is a generator object.|
| isInt8Array(value: Object): boolean | Checks whether the input value is of the **Int8Array** type.|
| isInt16Array(value: Object): boolean | Checks whether the input value is of the **Int16Array** type.|
| isInt32Array(value: Object): boolean | Checks whether the input value is of the **Int32Array** type.|
| isMap(value: Object): boolean | Checks whether the input value is of the **Map** type.|
| isMapIterator(value: Object): boolean | Checks whether the input value is of the **MapIterator** type.|
| isModuleNamespaceObject(value: Object): boolean | Checks whether the input value is a module namespace object.|
| isNativeError(value: Object): boolean | Checks whether the input value is of the **Error** type.|
| isNumberObject(value: Object): boolean | Checks whether the input value is a number object.|
| isPromise(value: Object): boolean | Checks whether the input value is a promise.|
| isProxy(value: Object): boolean | Checks whether the input value is a proxy.|
| isRegExp(value: Object): boolean | Checks whether the input value is a regular expression.|
| isSet(value: Object): boolean | Checks whether the input value is of the **Set** type.|
| isSetIterator(value: Object): boolean | Checks whether the input value is of the **SetIterator** type.|
| isSharedArrayBuffer(value: Object): boolean | Checks whether the input value is of the **SharedArrayBuffer** type.|
| isStringObject(value: Object): boolean | Checks whether the input value is a string object.|
| isSymbolObject(value: Object): boolean | Checks whether the input value is a symbol object.|
| isTypedArray(value: Object): boolean | Checks whether the input value is of the **TypedArray** type.|
| isUint8Array(value: Object): boolean | Checks whether the input value is of the **Uint8Array** type.|
| isUint8ClampedArray(value: Object): boolean | Checks whether the input value is of the **Uint8ClampedArray** type.|
| isUint16Array(value: Object): boolean | Checks whether the input value is of the **Uint16Array** type.|
| isUint32Array(value: Object): boolean | Checks whether the input value is of the **Uint32Array** type.|
| isWeakMap(value: Object): boolean | Checks whether the input value is of the **WeakMap** type.|
| isWeakSet(value: Object): boolean | Checks whether the input value is of the **WeakSet** type.|

Each format specifier in **printf()** is replaced with the converted value of the corresponding parameter. The following format specifiers are supported:
| Format Specifier| Description|
| -------- | -------- |
|    %s:  | String. It is used to convert all values except **BigInt**, **Object**, and **-0**.|
|    %d:  | Number. It is used to convert all values except **BigInt** and **Symbol**.|
|    %i:  | **parseInt(value, 10)**. It used to convert all values except **BigInt** and **Symbol**.|
|    %f:  | **parseFloat(value)**. It is used to convert all values except **Symbol**.|
|    %j:  | JSON. A circular reference in the parameter will be replaced with the string **'[Circular]'**.|
|    %o:  | A common JavaScript object represented in a string. It is similar to **util.inspect()** with the option **{ showHidden: true, showProxy: true }**. This will display a complete object, including innumerable properties and proxies.|
|    %O:  | Object. A string representation of an object with a common JavaScript object format. It is similar to **util.inspect()** without options. This will display a complete object, excluding innumerable properties and proxies.|
|    %c:  | This specifier is ignored and will jump any incoming CSS.|
|    %%:  | Single percent sign ('%'). This does not consume to-be-sampled parameters.|

#### How to Use
Use the APIs as follows:

1. readonly encoding()

```
import util from '@ohos.util'
var textEncoder = new util.TextEncoder();
var getEncoding = textEncoder.encoding();
```
2. encode()
```
import util from '@ohos.util'
var textEncoder = new util.TextEncoder();
var result = textEncoder.encode('abc');
```
3. encodeInto()
```
import util from '@ohos.util'
var textEncoder = new util.TextEncoder();
var obj = textEncoder.encodeInto('abc', dest);
```
4. textDecoder()
```
import util from '@ohos.util'
var textDecoder = new util.textDecoder("utf-16be", {fatal : true, ignoreBOM : false});
```
5. readonly encoding()
```
import util from '@ohos.util'
var textDecoder = new util.textDecoder("utf-16be", {fatal : true, ignoreBOM : false});
var getEncoding = textDecoder.encoding();
```
6. readonly fatal()
```
import util from '@ohos.util'
var textDecoder = new util.textDecoder("utf-16be", {fatal : true, ignoreBOM : false});
var fatalStr = textDecoder.fatal();
```
7. readonly ignoreBOM()
```
import util from '@ohos.util'
var textDecoder = new util.textDecoder("utf-16be", {fatal : true, ignoreBOM : false});
var ignoreBom = textDecoder.ignoreBOM();
```
8. decode()
```
import util from '@ohos.util'
var textDecoder = new util.textDecoder("utf-16be", {fatal : true, ignoreBOM : false});
var result = textDecoder.decode(input, {stream : true});
```
9. decodeWithStream()
```
import util from '@ohos.util'
var textDecoder = new util.textDecoder("utf-16be", {fatal : true, ignoreBOM : false});
var result = textDecoder.decodeWithStream(input, {stream : true});
```
10. printf()
```
import util from '@ohos.util'
var format = "%%%o%%%i%s";
var value =  function aa(){};
var value1 = 1.5;
var value2 = "qwer";
var result = util.printf(format,value,value1,value2);
```
11. getErrorString()
```
import util from '@ohos.util'
var errnum = 13;
var result = util.getErrorString(errnum);
```
12. callbackWrapper()
```
import util from '@ohos.util'
async function promiseFn() {
    return Promise.resolve('value');
};
var cb = util.callbackWrapper(promiseFn);
cb((err, ret) => {
    expect(err).strictEqual(null);
    expect(ret).strictEqual('value');
})
```
13. promiseWrapper()
```
import util from '@ohos.util'
function aysnFun(str1, str2, callback) {
    if (typeof str1 === 'string' && typeof str1 === 'string') {
        callback(null, str1 + str2);
    } else {
        callback('type err');
    }
}
let newPromiseObj = util.promiseWrapper(aysnFun)("Hello", 'World');
newPromiseObj.then(res => {
    expect(res).strictEqual('HelloWorld');
})
```
14. encodeSync()
```
import util from '@ohos.util'
var that = new util.Base64();
var array = new Uint8Array([115,49,51]);
var result = that.encodeSync(array);
```
15. encodeToStringSync()
```
import util from '@ohos.util'
var that = new util.Base64();
var array = new Uint8Array([115,49,51]);
var result = that.encodeToStringSync(array);
```
16. decodeSync()
```
import util from '@ohos.util'
var that = new util.Base64()
var buff = 'czEz';
var result = that.decodeSync(buff);

```
17. encode()
```
import util from '@ohos.util'
var that = new util.Base64()
var array = new Uint8Array([115,49,51]);
await that.encode(array).then(val=>{
})
done()
```
18. encodeToString()
```
import util from '@ohos.util'
var that = new util.Base64()
var array = new Uint8Array([115,49,51]);
await that.encodeToString(array).then(val=>{
})
done()
```
19. decode()
```
import util from '@ohos.util'
var that = new util.Base64()
var buff = 'czEz';
await that.decode(buff).then(val=>{
})
done()
```
20. createRationalFromString()
```
import util from '@ohos.util'
var pro = new util.RationalNumber(0, 0);
var res = pro.createRationalFromString("-1:2");
var result1 = res.valueOf();
```
21. compareTo()
```
import util from '@ohos.util'
var pro = new util.RationalNumber(2, 1);
var proc = new util.RationalNumber(3, 4);
var res = pro.compareTo(proc);
```
22. equals()
```
import util from '@ohos.util'
var pro = new util.RationalNumber(2, 1);
var proc = new util.RationalNumber(3, 4);
var res = pro.equals(proc);
```
23. valueOf()
```
import util from '@ohos.util'
var pro = new util.RationalNumber(2, 1);
var res = pro.valueOf();
```
24. getCommonDivisor()
```
import util from '@ohos.util'
var pro = new util.RationalNumber(0, 0);
var res = pro.getCommonDivisor(4, 8);
```
25. getDenominator()
```
import util from '@ohos.util'
var pro = new util.RationalNumber(2, 1);
var res = pro.getDenominator();
```
26. getNumerator()
```
import util from '@ohos.util'
var pro = new util.RationalNumber(-2, 1);
var res = pro.getNumerator();
```
27. isFinite()
```
import util from '@ohos.util'
var pro = new util.RationalNumber(-2, 1);
var res = pro.isFinite();
```
28. isNaN()
```
import util from '@ohos.util'
var pro = new util.RationalNumber(-2, 1);
var res = pro.isNaN();
```
29. isZero()
```
import util from '@ohos.util'
var pro = new util.RationalNumber(-2, 1);
var res = pro.isZero();
```
30. toString()
```
import util from '@ohos.util'
var pro = new util.RationalNumber(-2, 1);
var res = pro.toString();
```
31. updateCapacity()
```
import util from '@ohos.util'
var pro = new util.LruBuffer();
var result = pro.updateCapacity(100);
```
32. toString()
```
import util from '@ohos.util'
var pro = new util.LruBuffer();
pro.put(2,10);
pro.get(2);
pro.remove(20);
var result = pro.toString();
```
33. values()
```
import util from '@ohos.util'
var pro = new util.LruBuffer();
pro.put(2,10);
pro.put(2,"anhu");
pro.put("afaf","grfb");
var result = pro.values();
```
34. length
```
import util from '@ohos.util'
var pro = new util.LruBuffer();
pro.put(2,10);
pro.put(1,8);
var result = pro.length;
```
35. getCapacity()
```
import util from '@ohos.util'
var pro = new util.LruBuffer();
var result = pro.getCapacity();
```
36. clear()
```
import util from '@ohos.util'
var pro = new util.LruBuffer();
pro.put(2,10);
pro.clear();
```
37. getCreateCount()
```
import util from '@ohos.util'
var pro = new util.LruBuffer();
pro.put(1,8);
var result = pro.getCreateCount();
```
38. getMissCount()
```
import util from '@ohos.util'
var pro = new util.LruBuffer();
pro.put(2,10);
pro.get(2)
var result = pro.getMissCount();
```
39. getRemovalCount()
```

import util from '@ohos.util'
var pro = new util.LruBuffer();
pro.put(2,10);
pro.updateCapacity(2);
pro.put(50,22);
var result = pro.getRemovalCount();

```
40. getMatchCount()
```
import util from '@ohos.util'
var pro = new util.LruBuffer();
pro.put(2,10);
pro.get(2);
var result = pro.getMatchCount();
```
41. getPutCount()
```
import util from '@ohos.util'
var pro = new util.LruBuffer();
pro.put(2,10);
var result = pro.getPutCount();
```
42. isEmpty()
```
import util from '@ohos.util'
var pro = new util.LruBuffer();
pro.put(2,10);
var result = pro.isEmpty();
```
43. get()

```
import util from '@ohos.util'
var pro = new util.LruBuffer();
pro.put(2,10);
var result = pro.get(2);
```
44. put()
```
import util from '@ohos.util'
var pro = new util.LruBuffer();
var result = pro.put(2,10);
```
45. keys()
```
import util from '@ohos.util'
var pro = new util.LruBuffer();
pro.put(2,10);
var result = pro.keys();
```
46. remove()
```
import util from '@ohos.util'
var pro = new util.LruBuffer();
pro.put(2,10);
var result = pro.remove(20);
```
47. contains()
```
import util from '@ohos.util'
var pro = new util.LruBuffer();
pro.put(2,10);
var result = pro.contains(20);
```
48. createDefault()
```
import util from '@ohos.util'
var pro = new util.LruBuffer();
var result = pro.createDefault(50);
```
49. entries()
```
import util from '@ohos.util'
var pro = new util.LruBuffer();
pro.put(2,10);
var result = pro.entries();
```
50. \[Symbol.iterator\]()
```
import util from '@ohos.util'
var pro = new util.LruBuffer();
pro .put(2,10);
var result = pro[symbol.iterator]();
```
51. afterRemoval()
```
import util from '@ohos.util'
var arr = [ ];
class ChildLruBuffer extends util.LruBuffer
{
    constructor()
    {
        super();
    }
    static getInstance()
    {
        if(this.instance ==  null)
        {
            this.instance = new ChildLruBuffer();
        }
        return this.instance;
    }
    afterRemoval(isEvict, key, value, newValue)
    {
        if (isEvict === false)
        {
            arr = [key, value, newValue];
        }
    }
}
ChildLruBuffer.getInstance().afterRemoval(false,10,30,null)
```
A new constructor of **Scope** to implement the **compareTo** method.

```
class Temperature {
    constructor(value) {
        this._temp = value;
    }
    compareTo(value) {
        return this._temp >= value.getTemp();
    }
    getTemp() {
        return this._temp;
    }
    toString() {
        return this._temp.toString();
    }
}
```

52. constructor()

```
var tempLower = new Temperature(30);
var tempUpper = new Temperature(40);
var range = new Scope(tempLower, tempUpper);
```

53. toString()

```
var tempLower = new Temperature(30);
var tempUpper = new Temperature(40);
var range = new Scope(tempLower, tempUpper);
var result = range.toString() // => [30,40]
```

54. intersect()

```
var tempLower = new Temperature(30);
var tempUpper = new Temperature(40);
var range = new Scope(tempLower, tempUpper);
var tempMiDF = new Temperature(35);
var tempMidS = new Temperature(39);
var rangeFir = new Scope(tempMiDF, tempMidS);
var result = range.intersect(rangeFir)  // => [35,39]
```

55. intersect()

```
var tempLower = new Temperature(30);
var tempUpper = new Temperature(40);
var tempMiDF = new Temperature(35);
var tempMidS = new Temperature(39);
var range = new Scope(tempLower, tempUpper);
var result = range.intersect(tempMiDF, tempMidS)  // => [35,39]
```

56. getUpper()

```
var tempLower = new Temperature(30);
var tempUpper = new Temperature(40);
var range = new Scope(tempLower, tempUpper);
var result = range.getUpper() // => 40
```

57. getLower()

```
var tempLower = new Temperature(30);
var tempUpper = new Temperature(40);
var range = new Scope(tempLower, tempUpper);
var result = range.getLower() // => 30
```

58. expand()

```
var tempLower = new Temperature(30);
var tempUpper = new Temperature(40);
var tempMiDF = new Temperature(35);
var tempMidS = new Temperature(39);
var range = new Scope(tempLower, tempUpper);
var result = range.expand(tempMiDF, tempMidS)  // => [30,40]
```

59. expand()

```
var tempLower = new Temperature(30);
var tempUpper = new Temperature(40);
var tempMiDF = new Temperature(35);
var tempMidS = new Temperature(39);
var range = new Scope(tempLower, tempUpper);
var rangeFir = new Scope(tempMiDF, tempMidS);
var result = range.expand(rangeFir) // => [30,40]
```

60. expand()

```
var tempLower = new Temperature(30);
var tempUpper = new Temperature(40);
var tempMiDF = new Temperature(35);
var range = new Scope(tempLower, tempUpper);
var result = range.expand(tempMiDF)  // => [30,40]
```

61. contains()

```
var tempLower = new Temperature(30);
var tempUpper = new Temperature(40);
var tempMiDF = new Temperature(35);
var range = new Scope(tempLower, tempUpper);
var result = range.contains(tempMiDF) // => true
```

62. contains()

```
var tempLower = new Temperature(30);
var tempUpper = new Temperature(40);
var range = new Scope(tempLower, tempUpper);
var tempLess = new Temperature(20);
var tempMore = new Temperature(45);
var rangeSec = new Scope(tempLess, tempMore);
var result = range.contains(rangeSec) // => true
```

63. clamp()

```
var tempLower = new Temperature(30);
var tempUpper = new Temperature(40);
var tempMiDF = new Temperature(35);
var range = new Scope(tempLower, tempUpper);
var result = range.clamp(tempMiDF) // => 35
```
64. isAnyArrayBuffer()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isAnyArrayBuffer(new ArrayBuffer([]))
```
65. isArrayBufferView()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isArrayBufferView(new DataView(new ArrayBuffer(16)));
```
66. isArgumentsObject()
```
import util from '@ohos.util'
function foo() {
        var result = proc.isArgumentsObject(arguments);
    }
var f = foo();
```
67. isArrayBuffer()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isArrayBuffer(new ArrayBuffer([]));
```
68. isAsyncFunction()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isAsyncFunction(async function foo() {});
```
69. isBigInt64Array()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isBigInt64Array(new Int16Array([]));
```
70. isBigUint64Array()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isBigUint64Array(new Int16Array([]));
```
71. isBooleanObject()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isBooleanObject(new Boolean(false));
```
72. isBoxedPrimitive()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isBoxedPrimitive(new Boolean(false));
```
73. isDataView()
```
import util from '@ohos.util'
var proc = new util.Types();
const ab = new ArrayBuffer(20);
var result = proc.isDataView(new DataView(ab));
```
74. isDate()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isDate(new Date());
```
75. isExternal()
```
import util from '@ohos.util'
const data = util.createExternalType();
var reult13 = proc.isExternal(data);
```
76. isFloat32Array()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isFloat32Array(new Float32Array([]));
```
77. isFloat64Array()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isFloat64Array(new Float64Array([]));
```
78. isGeneratorFunction()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isGeneratorFunction(function* foo() {});
```
79. isGeneratorObject()
```
import util from '@ohos.util'
var proc = new util.Types();
function* foo() {}
const generator = foo();
var result = proc.isGeneratorObject(generator);
```
80. isInt8Array()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isInt8Array(new Int8Array([]));
```
81. isInt16Array()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isInt16Array(new Int16Array([]));
```
82. isInt32Array()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isInt32Array(new Int32Array([]));
```
83. isMap()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isMap(new Map());
```
84. isMapIterator()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isMapIterator(map.keys());
```
85. isModuleNamespaceObject()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isModuleNamespaceObject(util);
```
86. isNativeError()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isNativeError(new TypeError());
```
87. isNumberObject()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isNumberObject(new Number(0));
```
88. isPromise()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isPromise(Promise.resolve(42));
```
89. isProxy()
```
import util from '@ohos.util'
var proc = new util.Types();
const target = {};
const proxy = new Proxy(target, {});
var result = proc.isProxy(proxy);
```
90. isRegExp()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isRegExp(new RegExp('abc'));
```
91. isSet()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isSet(new Set());
```
92. isSetIterator()
```
import util from '@ohos.util'
var proc = new util.Types();
const set = new Set();
var result = proc.isSetIterator(set.keys());
```
93. isSharedArrayBuffer()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isSharedArrayBuffer(new ArrayBuffer([]));
```
94. isStringObject()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isStringObject(new String('foo'));
```
95. isSymbolObject()
```
import util from '@ohos.util'
var proc = new util.Types();
const symbols = Symbol('foo');
var result = proc.isSymbolObject(Object(symbols));
```
96. isTypedArray()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isTypedArray(new Float64Array([]));
```
97. isUint8Array()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isUint8Array(new Uint8Array([]));
```
98. isUint8ClampedArray()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isUint8ClampedArray(new Uint8ClampedArray([]));
```
99. isUint16Array()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isUint16Array(new Uint16Array([]));
```
100. isUint32Array()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isUint32Array(new Uint32Array([]));
```
101. isWeakMap()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isWeakMap(new WeakMap());
```
102. isWeakSet()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isWeakSet(new WeakSet());
```

## js_sys_module
###  Introduction
js_sys_module provides the following classes:<br>**Process**: provides APIs to obtain multiple IDs of a process, obtain and modify the working directory of a process, and exit and stop a process. **childprocess**:  provides APIs for a process to obtain the standard input and output of its child processes, send signals, and close its child processes.
### Directory Structure

```
commonlibrary/ets_utils/js_sys_module/
├── Class:PROCESS                   # Process class
├── Uid                             # UID attribute
├── Gid                             # GID attribute
├── EUid                            # EUID attribute
├── EGid                            # EGID attribute
├── Groups                          # Groups attribute
├── Pid                             # PID attribute
├── Ppid                            # PPID attribute
├── chdir()                         # chdir method
├── uptime()                        # uptime method
├── kill()                          # kill method
├── abort()                         # abort method
├── on()                            # on method
├── tid                             # tid method
├── getStartRealtime()              # getStartRealtime method
├── getAvailableCores()             # getAvailableCores method
├── getPastCputime()                # getPastCputime method
├── isIsolatedProcess()             # isIsolatedProcess method
├── is64Bit()                       # is64Bit method
├── isAppUid()                      # isAppUid method
├── getUidForName()                 # getUidForName method
├── getThreadPriority()             # getThreadPriority method
├── getSystemConfig()               # getSystemConfig method
├── getEnvironmentVar()             # getEnvironmentVar method
├── exit()                          # exit method
├── cwd()                           # cwd method
├── off()                           # off method
├── runCmd()                        # runCmd method
└─── Class:CHILDPROCESS             # ChildProcess class
    ├── close()                     # close method
    ├── kill()                      # kill method
    ├── getOutput()                 # getOutput method
    ├── getErrorOutput()            # getErrorOutput method
    ├── wait()                      # wait method
    ├── killed                      # killed attribute
    ├── pid                         # PID attribute
    ├── ppid                        # PPID attribute
    └── exitCode                    # exitCode attribute
|—— Class:CONSOLE 
    ├── debug()                     # debug method
    ├── log()                       # log method
    ├── info()                      # info method
    ├── warn()                      # warn method
    ├── error()                     # error method
    ├── assert()                    # assert method
    ├── count()                     # count method
    ├── countReset()                # countReset method
    ├── dir()                       # dir method
    ├── dirxml()                    # dirxml method
    ├── group()                     # group method
    ├── groupCollapsed()            # groupCollapsed method
    ├── groupEnd()                  # groupEnd method
    ├── table()                     # table method
    ├── time()                      # time method
    ├── timeEnd()                   # timeEnd method
    ├── timeLog()                   # timeLog method
    ├── trace()                     # trace method
|—— Class:TIMER 
    ├── setInterval()               # setInterval method
    ├── setTimeout()                # setTimeout method
    ├── clearInterval()             # clearInterval method
    ├── clearTimeout()              # clearTimeout method
```

### How to Use

#### Available APIs
| API| Description|
| -------- | -------- |
| const uid :number | Obtains the user identifier (UID) of this process.|
| const gid :number | Obtains the group identifier (GID) of this process.|
| const euid :number | Obtains the effective user identifier (EUID) of this process.|
| const egid :number | Obtains the effective group identifier (EGID) of this process.|
| const groups :number[] |  Obtains an array with supplementary group IDs.|
| const pid :number | Obtains the process ID (PID) of this process.|
| const ppid :number |  Obtains the parent process ID (PPID) of this process.|
| chdir(dir:string) :void | Changes the working directory of this process.|
| uptime() :number |  Obtains the running time of this process, in seconds.|
| Kill(pid:number, signal:number) :boolean | Sends a signal to the specified process to terminate it. The value **true** means that the signal is sent.|
| abort() :void | Aborts a process and generates a core file. It will cause the process to exit immediately.|
| on(type:string ,listener:EventListener) :void | Stores the events triggered by the user.|
| exit(code:number):void | Terminates this process.|
| cwd():string |  Obtains the working directory of this process.|
| off(type: string): boolean | Deletes the event stored by the user. The value **true** means that the event is deleted.|
| runCmd(command: string, options?: { timeout : number, killSignal : number \| string, maxBuffer : number }): ChildProcess |Forks a new process to run a shell command and returns the **ChildProcess** object. **command** indicates the shell command to run, and **options** indicates certain running parameters, such as **timeout**, **killsignal**, and **maxbuffer**, of the child process. If **timeout** is set, the child process sends the **kill** signal after the specified timeout reaches. **maxbuffer** is used to limit the maximum size of **stdout** and **stderr** that can be received.|
| wait(): Promise\<number>| Waits until the child process ends. It uses a promise to return the exit code of the child process.|
| getOutput(): Promise\<Uint8Array> |  Obtains the standard output of the child process.|
| getErrorOutput(): Promise\<Uint8Array> | Obtains the standard error output of the child process.|
| const tid:number | Obtains the TID of the process.|
| getStartRealtime() :number | Obtains the duration (in milliseconds) from the time the system starts to the time the process starts.|
| getAvailableCores() :number[] | Obtains the number of CPU cores available for the process on a multi-core device.|
| getPastCputime() :number | Obtains the CPU time (in milliseconds) from the time the process starts to the current time.|
| isIsolatedProcess(): boolean | Checks whether this process is isolated.|
| is64Bit(): boolean | Checks whether this process is running in a 64-bit environment.|
| isAppUid(v:number): boolean | Checks whether a UID belongs to the current app.|
| getUidForName(v:string): number | Obtains the process UID based on the process name.|
| getThreadPriority(v:number): number | Obtains the thread priority based on the TID.|
| getSystemConfig(name:number): number | Obtains the system configuration based on the configuration name.|
| getEnvironmentVar(name:string): string | Obtains the value of an environment variable.|
| close(): void | Closes the child process in running.|
| kill(signal: number \| string): void |  Sends a signal to the specified child process to terminate it.|
| readonly killed: boolean | Specifies whether the signal is sent. The value **true** means that the signal is sent.|
| readonly exitCode: number | Indicates the exit code of the child process.|
| readonly pid: number | Indicates the PID of the child process.|
| readonly ppid: number | Indicates the PPID of the process.|
| debug(message: string, ...arguments: any[]): void | print debug information. |
| log(message: string, ...arguments: any[]): void | print log information. |
| info(message: string, ...arguments: any[]): void | print info information. |
| warn(message: string, ...arguments: any[]): void | print warn information. |
| error(message: string, ...arguments: any[]): void | print error information. |
| assert(value?: Object, ...arguments: Object[]): void | if value is false，print arguments information. |
| count(label?: string): void | Counts the label name. |
| countReset(label?: string): void | Clear the count of label names. |
| dir(dir?: Object): void | Print the object content. |
| dirxml(...arguments: Object[]): void | print log information. |
| group(...arguments: Object[]): void | Indent one group |
| groupCollapsed(...arguments: Object[]): void | Indent one group |
| groupEnd(): void | Unindent one group. |
| table(tableData?: Object): void | Print data in tabular form. |
| time(label?: string): void | Start the clock. |
| timeEnd(label?: string): void | End the timer. |
| timeLog(label?: string, ...arguments: Object[]): void | Print current timing. |
| trace(...arguments: Object[]): void | Print current stack. |
|  setInterval(handler: Function \| string, delay: number, ...arguments: any[]): number | Timing to call callback function. |
|  setTimeout(handler: Function \| string, delay?: number, ...arguments: any[]): number | Call the callback function when the timing ends. |
|  clearInterval(intervalID?: number): void | Clear the timing callback. |
|  clearTimeout(timeoutID?: number): void |Clear the timing callback. |

#### How to Use

Use the APIs as follows:
1. uid()
```
uid(){
    var res =  Process.uid;
}
```
2. gid()
```
gid(){
    var result = Process.gid;
}
```
3. euid()
```
euid(){
    var and = Process.euid;
}
```
4. egid()
```
egid(){
    var resb = Process.egid;
}
```
5. groups()
```
groups(){
    var answer = Process.groups;
}
```
6. pid()
```
pid(){
    var result = Process.pid;
}
```
7. ppid()
```
ppid(){
    var result = Process.ppid;
}
```
8. chdir()
```
chdir(){
    Process.chdir("123456");
}
```
9. uptime()
```
uptime(){
    var num = Process.uptime();
}
```
10. kill()
```
kill(){
    var ansu = Process.kill(5,23);
}
```
11. abort()
```
abort(){
    Process.abort();
}
```
12. on()
```
on(){
    function add(num){
        var value = num + 5;
        return value;
    }
    Process.on("add",add);
}
```
13. exit()
```
exit(){
    Process.exit(15);
}
```
14. Cwd()
```
Cwd(){
    var result = Process.cwd();
}
```
15. off()

```
off(){
    var result =  Process.off("add");
}
```
16. runCmd()
```
runCmd(){
    var child = process.runCmd('echo abc')
    // killSignal can be a number or string.
    var child = process.runCmd('echo abc;', {killSignal : 'SIGKILL'});
    var child = process.runCmd('sleep 5; echo abc;', {timeout : 1, killSignal : 9, maxBuffer : 2})
}
```
17. wait()
```
wait()
{
    var child = process.runCmd('ls')
    var status = child.wait();
    status.then(val => {
        console.log(val);
    })
}
```
18. getOutput()
```
getOutput(){
    var child = process.runCmd('echo bcd;');
    var res = child.getOutput();
    child.wait();
    res.then(val => {
        console.log(val);
    })
}
```
19. getErrorOutput()
```
getErrorOutput(){
    var child = process.runCmd('makdir 1.txt'); // execute an error command
    var res = child.getErrorOutput();
    child.wait();
    res.then(val => {
        console.log(val);
    })
}
```
20. close()
```
close(){
    var child =  process.runCmd('ls; sleep 5s;')
    var result = child.close()
}
```
21. kill()
```
kill(){
    var child =  process.runCmd('ls; sleep 5s;')
    var result = child.kill('SIGHUP');
    child.wait();
    var temp = child.killed;
}
```
22. killed
```
{
    var child = process.runCmd('ls; sleep 5;')
    child.kill(3);
    var killed_ = child.killed;
    child.wait();
}
```
23. exitCode
```
{
    var child = process.runCmd('ls; sleep 5;')
    child.kill(9);
    child.wait();
    var exitCode_ = child.exitCode;
}
```
24. pid
```
pid
{
    var child = process.runCmd('ls; sleep 5;')
    var pid_ = child.pid;
    child.wait();
}
```
25. ppid
```
ppid
{
    var child = process.runCmd('ls; sleep 5;')
    var ppid_ = child.ppid;
    child.wait();
}
```
26. tid
```
tid(){
    var ansu = Process.tid;
}
```
27. isIsolatedProcess()
```
isIsolatedProcess(){
    var ansu = Process.isIsolatedProcess()();
}
```
28. isAppUid()
```
isAppUid(){
    var ansu = Process.isAppUid(10000);
}
```
29. is64Bit()
```
is64Bit(){
    var ansu = Process.is64Bit();
}
```
30. getUidForName()
```
getUidForName(){
    var buf = "root";
    var ansu = Process.getUidForName(buf);
}
```
31. getEnvironmentVar()
```
getEnvironmentVar(){
    var ansu = Process.getEnvironmentVar('USER');
}
```
32. getAvailableCores()
```
getAvailableCores(){
    var ansu = Process.getAvailableCores();
}
```
33. getThreadPriority()
```
getThreadPriority(){
    var result = Process.getTid();
    var ansu = getThreadPriority(result);
}
```
34. getStartRealtime()
```
getStartRealtime(){
    var ansu = Process.getStartRealtime();
}
```
35. getPastCputime()
```
getPastCputime(){
    var ansu = Process.getPastCputime();
}
```
36. getSystemConfig()
```
getSystemConfig(){
    var _SC_ARG_MAX = 0;
    var ansu = Process.getSystemConfig(_SC_ARG_MAX)
}
```
37.console.debug()
```
console.debug("print debug log");
}
```
38.console.log()
```
console.debug("print log");
}
```
39.console.info()
```
console.debug("print info log");
}
```
40.console.warn()
```
console.debug("print warn log");
}
```
41.console.error()
```
console.debug("print error log");
}
```
42.console.assert()
```
for (let number = 2; number <= 5; number++) {
    console.assert(number % 2 === 0, "error");
}
```
43.console.count()
```
console.count("myObj");
```
44.console.countReset()
```
console.count("myObj");
console.countReset("myObj");
```
45.console.dir()
```
function cat(name, age, score){
  this.name = name;
  this.age = age;
  this.score = score;
}
var c = new cat("ohos", 2, [6,8,7]);
console.dir(c);
```
46.console.dirxml()
```
console.xml("print log");
```
47.console.group()
```
console.group();
```
48.console.groupCollapsed()
```
console.groupCollapsed();
```
49.console.groupEnd()
```
console.groupEnd();
```
50.console.table()
```
var languages = {
  csharp: { name: "C#", paradigm: "undefined" },
  fsharp: { name: "F#", paradigm: "functional" }
};
console.table(languages);
```
51.console.time()
```
console.time("timer1");
```
52.console.timeEnd()
```
console.time("timer1");
console.timeEnd("timer1");
```
53.console.timeLog()
```
console.time("timer1");
console.timeLog("timer1");
```
54.console.trace()
```
console.trace();
```
55.setInterval()
```
function callback() {
  console.log("setInterval");
};
setInterval(callback, 100);
```
55.setTimeout()
```
function callback() {
  console.log("setTimeout");
};
setTimeout(callback, 100);
```
55.clearInterval()
```
function callback() {
  console.log("clearInterval");
};
var myVar = setInterval(callback, 1000);
clearInterval(myVar);
```
56.clearTimeout()
```
function callback() {
  console.log("clearTimeout");
};
var myVar = setTimeout(callback, 1000);
clearTimeout(myVar);
```

## js_concurrent_module

### Introduction

The js_concurrent_module provides the worker thread, which communicates with the host thread through **postMessage**.

### Available APIs
For details about the API implementation, see js_worker_module/worker.

#### Worker Object

Object used by the host thread to communicate with the worker thread.

##### APIs

1. 

- API

|constructor(scriptURL:string, options?:WorkerOptions) | Constructor used to create a worker object.|
|---|---|

- Example

You can use any of the following methods to create a worker thread, depending on whether the **workers** directory that stores the **worker.ts** file is at the same level as the **pages** directory and whether the FA or stage model is in use:

(1) In the FA model, the **workers** and **pages** directories are at the same level.
```
import worker from "@ohos.worker";
const workerInstance = new worker.Worker("workers/worker.js", {name:"first worker"});
```
(2) In the FA model, the **workers** and **pages** directories are at different levels.
```
import worker from "@ohos.worker";
const workerInstance = new worker.Worker("../workers/worker.js", {name:"first worker"});
```
(3) In the stage model, the **workers** and **pages** directories are at the same level.
```
import worker from "@ohos.worker";
const workerInstance = new worker.Worker('entry/ets/workers/worker.ts');
```
(4) In the stage model, the **workers** and **pages** directories are at different levels.
```
import worker from "@ohos.worker";
const workerInstance = new worker.Worker('entry/ets/pages/workers/worker.ts');
```
scriptURL in the stage model — Description of "entry/ets/workers/worker.ts".
- **entry**: value of the **name** attribute under **module** in the **module.json5** file.
- **ets**: programming language in use.
- **worker.ts**: file for the module. You can also use a **worker.js** file.

Depending on whether the **workers** and **pages** directories are at the same level, you may need to configure the **buildOption** attribute in the **build-profile.json5** file.

(1) If the **workers** and **pages** directories are at the same level, the configuration is optional.

In the FA model:
```
  "buildOption": {
    "sourceOption": {
      "workers": [
        "./src/main/ets/MainAbility/workers/worker.js"
      ]
    }
  }
```
In the stage model:
```
  "buildOption": {
    "sourceOption": {
      "workers": [
        "./src/main/ets/workers/worker.ts"
      ]
    }
  }
```
(2) If the **workers** and **pages** directories are at different levels, the configuration is mandatory.

In the FA model:
```
  "buildOption": {
    "sourceOption": {
      "workers": [
        "./src/main/ets/workers/worker.js"
      ]
    }
  }
```
In the stage model (the following assumes that the **workers** directory is under the **pages** directory):
```
  "buildOption": {
    "sourceOption": {
      "workers": [
        "./src/main/ets/pages/workers/worker.ts"
      ]
    }
  }
```
2. 

- API

| postMessage(message:Object, options?:PostMessageOptions)  | Sends a message to the worker thread. |
|---|---|
| postMessage(message:Object, transfer:ArrayBuffer[])  | Sends a message to the worker thread. |

- Examples

```
// Example 1
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
worker.postMessage("hello world");
 
// Example 2
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
var buffer = new ArrayBuffer(8);
worker.postMessage(buffer, [buffer]);
```

3. 

- API

| on(type:string, listener:EventListener)  | Adds an event listener to the worker. |
|---|---|

- Examples

```
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
worker.on("alert", (e)=>{
     console.log("worker on...");
});
```

4. 

- API

| once(type:string, listener:EventListener)  | Adds an event listener to the worker and removes the event listener automatically after it is invoked once. |
|---|---|

- Examples

```
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
worker.once("alert", (e)=>{
    console.log("worker once...");
});
```

5. 

- API

| off(type:string, listener?:EventListener)  | Removes an event listener for the worker. |
|---|---|

- Examples

```
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
worker.off("alert");
```

6. 

- API

| terminate()  | Terminates the worker thread to stop the worker from receiving messages. |
|---|---|

- Example

```
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
worker.terminate();
```

7. 

- API

| removeEventListener(type:string, listener?:EventListener)  | Removes an event listener for the worker. |
|---|---|

- Example

```
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
worker.removeEventListener("alert", (e)=>{
    console.log("worker removeEventListener...");
});
```

8. 

- API

| dispatchEvent(event: Event)  | Dispatches the event defined for the worker. |
|---|---|

- Example

```
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
worker.dispatchEvent({type:"alert"});
```

9. 

- API

| removeAllListener()  | Removes all event listeners for the worker. |
|---|---|

- Example

```
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
worker.removeAllListener();
```

##### Attributes

1. 

- Attribute

| onexit?:(code:number)=>void  | Event handler to be called when the worker thread exits. The handler is executed in the host thread. |
|---|---|

- Example

```
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
worker.onexit = function(e) {
    console.log("onexit...");
}
```

2. 

- Attribute

| onerror?:(ev:ErrorEvent)=>void  | Event handler to be called when an exception occurs during worker execution. The event handler is executed in the host thread. |
|---|---|

- Example

```
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
worker.onerror = function(e) {
    console.log("onerror...");
}
```

3. 

- Attribute

| onmessage?:(ev:MessageEvent)=>void  | Event handler to be called when the host thread receives a message sent by the worker thread through **parentPort.postMessage**. The event handler is executed in the host thread.|
|---|---|

- Example

```
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
worker.onmessage = function(e) {
    console.log("onmessage...");
}
```

4. 

- Attribute

| onmessageerror?:(event:MessageEvent)=>void  | Event handler to be called when the worker thread receives a message that cannot be serialized. The event handler is executed in the host thread.|
|---|---|

- Example

```
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
worker.onmessageerror = function(e) {
    console.log("onmessageerror...");
}
```

#### parentPort Object

Object of the worker thread used to communicate with the host thread.

##### APIs

1. 

- API

| postMessage(message:Object, options?:PostMessageOptions)  | Sends a message to the host thread from the worker thread.|
|---|---|
| postMessage(message:Object, transfer:ArrayBuffer[])  | Sends a message to the host thread from the worker thread. |

- Example

```
// main.js
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
worker.postMessage("hello world");

// worker.js
import worker from "@ohos.worker"
const parentPort = worker.parentPort;
parentPort.onmessage = function(e) {
    parentPort.postMessage("hello world from worker.js");
}
```

2. 

- API

| close()  | Terminates the worker thread to stop the worker from receiving messages. |
|---|---|

- Example

```
// main.js
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
worker.postMessage("hello world");

// worker.js
import worker from "@ohos.worker"
const parentPort = worker.parentPort;
parentPort.onmessage = function(e) {
    parentPort.close();
}
```

##### Attributes

1. 

- Attribute

| onmessage?:(event:MessageEvent)=>void  | Event handler to be called when the host thread receives a message sent by the worker thread through **parentPort.postMessage**. The event handler is executed in the worker thread. |
|---|---|

- Example

```
// main.js
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
worker.postMessage("hello world");

// worker.js
import worker from "@ohos.worker"
const parentPort = worker.parentPort;
parentPort.onmessage = function(e) {
    console.log("receive main.js message");
}
```

2. 

- Attribute

| onerror?:(ev: ErrorEvent)=>void  | Event handler to be called when an exception occurs during worker execution. The event handler is executed in the worker thread. |
|---|---|

- Example

```
// main.js
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
worker.postMessage("hello world");

// worker.js
import worker from "@ohos.worker"
const parentPort = worker.parentPort;
parentPort.onerror = function(e) {
    console.log("onerror...");
}

```

3. 

- Attribute

| onmessageerror?:(event: MessageEvent)=>void  | Event handler to be called when the worker thread receives a message that cannot be deserialized. The event handler is executed in the worker thread. |
|---|---|

- Example

```
// main.js
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
worker.postMessage("hello world");

// worker.js
import worker from "@ohos.worker"
const parentPort = worker.parentPort;
parentPort.onmessageerror = function(e) {
    console.log("onmessageerror...");
}
```


# Repositories Involved

[arkcompiler_ets_runtime](https://gitee.com/openharmony/arkcompiler_ets_runtime/blob/master/README_zh.md)
[arkui_ace_engine](https://gitee.com/openharmony/arkui_ace_engine/blob/master/README_zh.md)
[arkui_napi](https://gitee.com/openharmony/arkui_napi/blob/master/README_zh.md)

# Licenses

**js_api_module** can be used under the [Mozilla Public License](https://www.mozilla.org/en-US/MPL/). For the complete license text, see [License](https://gitee.com/openharmony/commonlibrary_ets_utils/blob/master/js_api_module/mozilla_docs.txt).

**js_util_module** can be used under the [Mozilla Public License](https://www.mozilla.org/en-US/MPL/). For the complete license text, see [License](https://gitee.com/openharmony/commonlibrary_ets_utils/blob/master/js_util_module/mozilla_docs.txt).

**js_concurrent_module** can be used under the [Mozilla Public License](https://www.mozilla.org/en-US/MPL/). For the complete license text, see [License](https://gitee.com/openharmony/commonlibrary_ets_utils/blob/master/js_concurrent_module/mozilla_docs.txt).

**js_sys_module** can be used under the [Mozilla Public License](https://www.mozilla.org/en-US/MPL/). For the complete license text, see [License](https://gitee.com/openharmony/commonlibrary_ets_utils/blob/master/js_sys_module/mozilla_docs.txt).
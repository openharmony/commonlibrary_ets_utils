# ets_utils

-   [Submodule](#Submodule)
    -   [1.js_api_module submodule](#1.js_api_module submodule)
        -   [1.1.Introduction](#1.1.Introduction)
        -   [1.2.Contents](#1.2.Contents)
        -   [1.3.Illustrate](#1.3.Illustrate)
            -   [1.3.1.Interface Description](#1.3.1.Interface Description)
            -   [1.3.2.Instructions for use](#1.3.2.Instructions for use)
    -   [2.js_util_module submodule](#2.js_util_module submodule)
        -   [2.1.Introduction](#2.1.Introduction)
        -   [2.2.Contents](#2.2.Contents)
        -   [2.3.Illustrate](#2.3.Illustrate)
            -   [2.3.1.Interface Description](#2.3.1.Interface Description)
            -   [2.3.2.Instructions for use](#2.3.2.Instructions for use)
    -   [3.js_sys_module submodule](#3.js_sys_module submodule)
        -   [3.1.Introduction](#3.1.Introduction)
        -   [3.2.Contents](#3.2.Contents)
        -   [3.3.Illustrate](#3.3.Illustrate)
            -   [3.3.1.Interface Description](#3.3.1.Interface Description)
            -   [3.3.2.Instructions for use](#3.3.2.Instructions for use)
    -   [4.js_concurrent_module submodule](#4.js_concurrent_module submodule)
        -   [4.1.Introduction](#4.1.Introduction)
        -   [4.2.Interface description](#4.2.Interface description)
            -   [4.2.1.Worker object description](#4.2.1.Worker object description)
                -   [4.2.1.1.Interface](#4.2.1.1.Interface)
                -   [4.2.1.2.Attribute](#4.2.1.2.Attribute)
            -   [4.2.2.parentPort object description](#4.2.2.parentPort object description)
                -   [4.2.2.1.Interface](#4.2.2.1.Interface)
                -   [4.2.2.2.Attribute](#4.2.2.2.Attribute)
-   [Related warehouse](#Related warehouse)
-   [License](#License)

## 1.js_api_module_submodule
### 1.1.Introduction

The interface of URL is used to parse, construct, normalize, and encode URLs. The constructor of URL creates a new URL object. In order to make changes to the resolved components of the URL or to the URL. The URLSearchParams interface defines some practical methods to process URL query strings.

URI Represents a Uniform Resource Identifier (URI) reference.

XML representation refers to extensible markup language.

The XmlSerializer interface is used to generate an xml file. The constructor of XmlSerializer creates a new XmlSerializer object, and calls the method of the XmlSerializer object to generate an xml file. The XmlPullParser interface is used to parse the existing xml file. The constructor of XmlPullParser creates a new XmlPullParser object, and calls the method of the XmlPullParser object to parse the xml file.

### 1.2.Contents

```
commomlibrary/ets_utils/js_api_module/
├── Class:URL                                                                # URL class
│   ├── new URL(input[, base])                                               # Create URL object
│   ├── hash                                                                 # hash attribute
│   ├── host                                                                 # host attribute
│   ├── hostname                                                             # hostname attribute
│   ├── href                                                                 # href attribute
│   ├── origin                                                               # origin attribute
│   ├── password                                                             # password attribute
│   ├── pathname                                                             # pathname attribute
│   ├── port                                                                 # port attribute
│   ├── protocol                                                             # protocol attribute
│   ├── search                                                               # search attribute
│   ├── searchParams                                                         # searchParams attribute
│   ├── username                                                             # username attribute
│   ├── toString()                                                           # toString method
│   └── toJSON()                                                             # toJSON method
├── Class: URLSearchParams                                                   # URLSearchParams class
│   ├── new URLSearchParams()                                                # Create URLSearchParams object
│   ├── new URLSearchParams(string)                                          # Create URLSearchParams object
│   ├── new URLSearchParams(obj)                                             # Create URLSearchParams object
│   ├── new URLSearchParams(iterable)                                        # Create URLSearchParams object
│   ├── append(name, value)                                                  # append method
│   ├── delete(name)                                                         # delete method
│   ├── entries()                                                            # entries method
│   ├── forEach(fn[, thisArg])                                               # forEach method
│   ├── get(name)                                                            # get method
│   ├── getAll(name)                                                         # getAll method
│   ├── has(name)                                                            # has method
│   ├── keys()                                                               # keys method
│   ├── set(name, value)                                                     # set method
│   ├── sort()                                                               # sort method
│   ├── toString()                                                           # toString method
│   ├── values()                                                             # values method
│   └── urlSearchParams[Symbol.iterator]()                                   # Create URLSearchParams object
├── Class:URI                                                                # URI class
│   ├── URI(str: string)                                                     # Create URI object
│   ├── scheme                                                               # scheme attribute
│   ├── authority                                                            # authority attribute
│   ├── ssp                                                                  # ssp attribute
│   ├── userinfo                                                             # userinfo attribute
│   ├── host                                                                 # host attribute
│   ├── port                                                                 # port attribute
│   ├── query                                                                # query attribute
│   ├── fragment                                                             # fragment attribute
│   ├── path                                                                 # path method
│   ├── equals(ob: Object)                                                   # equals method
│   ├── normalize()                                                          # normalize method
│   ├── checkIsAbsolute()                                                    # checkIsAbsolute method
│   ├── normalize()                                                          # normalize method
│   ├── toString()                                                           # toString method
├── Class:ConvertXml                                                         # ConvertXml class
│   ├── ConvertXml()                                                         # Create convertxml class object
│   └── convert(xml: string, options: Object)                                # convert method
├── Class:XmlSerializer                                                      # XmlSerializer class
│   ├── new XmlSerializer(buffer: ArrayBuffer | DataView, encoding?: string) # Create XmlSerializer class object
│   ├── setAttributes(name: string, value: string)                           # Set Attributes method
│   ├── addEmptyElement(name: string)                                        # Add an empty element method
│   ├── setDeclaration()                                                     # Set the Declaration method
│   ├── startElement(name: string)                                           # Set the start element method
│   ├── endElement()                                                         # Set the end element method
│   ├── setNamespace(prefix: string, namespace: string)                      # Namespace method
│   ├── setCommnet(text: string)                                             # Set up Commnet method
│   ├── setCData(text: string)                                               # Set the CData method
│   ├── setText(text: string)                                                # Set the Text method
│   ├── setDocType(text: string)                                             # Set DocType method
└── Class: XmlPullParser                                                     # XmlPullParser class
    ├── new (buffer: ArrayBuffer | DataView, encoding?: string)              # Create XmlPullParser object
    └── parse(option: ParseOptions)                                          # parse method
```

### 1.3.Illustrate

#### 1.3.1.Interface Description


| Interface name | Illustrate                                                         |
| -------- | -------- |
| new URL(url: string,base?:string \| URL) | Create and return a URL object that references the URL specified by the absolute URL string, the relative URL string, and the basic URL string. |
| tostring():string | The stringification method returns a USVString containing the complete URL. It is equivalent to the read-only URL.href. |
| toJSON():string | This method returns a USVString, which contains a serialized URL version. |
| new URLSearchParams() | The URLSearchParams() constructor has no parameters. This method creates and returns a new URLSearchParams object. The beginning'?' character will be ignored. |
| new URLSearchParams(string) | The input parameter of URLSearchParams(string) constructor is string data type. This method creates and returns a new URLSearchParams object. The beginning'?' character will be ignored. |
| new URLSearchParams(obj) | URLSearchParams(obj) The input parameter of the constructor is the obj data type. This method creates and returns a new URLSearchParams object. The beginning'?' character will be ignored. |
| new URLSearchParams(iterable) | URLSearchParams(iterable) The input parameter of the constructor is the iterable data type. This method creates and returns a new URLSearchParams object. The beginning'?' character will be ignored. |
| has(name: string): boolean | Retrieve whether the searchParams object contains name. If yes, it returns true, otherwise it returns false. |
| set(name: string, value string): void |  Retrieve whether the searchParams object contains a key-value pair whose key is name. If not, add the key-value pair, if any, modify the value corresponding to the first key in the object, and delete the remaining key-value pairs whose key is name. |
| sort(): void | According to the Unicode code point of the key, sort all key/value pairs contained in this object and return undefined. |
| toString(): string | According to the searchParams object, the query string applicable in the URL is returned. |
| keys(): iterableIterator\<string> | Return an iterator, which allows iterating through all the key values contained in the object. |
| values(): iterableIterator\<string> | Returns an iterator, which allows iterating over all the value values contained in the object. |
| append(name: string, value: string): void | Insert the name, value key-value pair in the searchParams object. |
| delete(name: string): void | Traverse the searchParams object, find all the names, and delete the corresponding key-value pairs. |
| get(name: string): string | Retrieve the first name in the searchParams object and return the value corresponding to the name key. |
| getAll(name: string): string[] | Retrieve all names in the searchParams object and return all the values corresponding to the name key. |
| entries(): iterableIterator<[string, string]> | Returns an iterator that allows iterating through all key/value pairs contained in the searchParams object. |
| forEach(): void | Through the callback function to traverse the key-value pairs on the URLSearchParams instance object. |
| urlSearchParams\[Symbol.iterator]() | Returns an ES6 iterator for each name-value pair in the query string. Each item of the iterator is a JavaScript array. |
| URI​(str: string) | Construct the URI by parsing the given input parameter (String str). This constructor parses the given string strictly in accordance with the grammatical provisions in RFC 2396 Appendix A. |
| getScheme​() | Return the scheme component of this URI, or null if the scheme is not defined. |
| getAuthority​() | Returns the decoded authority component of this URI, or null if authority is not defined. The string returned by this method is the same as the string returned by the getRawAuthority method, except that all escaped octet sequences are decoded. |
| getSchemeSpecificPart​() |  Returns the decoding scheme-specific part of this URI. The string returned by this method is the same as the string returned by the getRawSchemeSpecificPart method, except that all escaped octet sequences are decoded. |
| getUserInfo​() | Returns the decoded userinfo component of this URI. The userinfo component of the URI (if defined) only contains characters in unreserved, punctuation, escape, and other categories. |
| getHost​() | Return the host component of this URI, or null if host is not defined. |
| getPort​() | Return the port of this URI, or -1 if the port is not defined. The port component of the URI (if defined) is a non-negative integer. |
| getQuery​() | Returns the decoded query component of this URI, or null if the query is not defined. The string returned by this method is the same as the string returned by the getRawQuery method, except that all escaped octet sequences are decoded. |
| getFragment​() | Returns the decoded fragment component of this URI, or null if the fragment is not defined. The string returned by this method is the same as the string returned by the getRawFragment method, except that all escaped octet sequences are decoded. |
| getPath​() | Returns the decoded path component of this URI, or null if path is not defined. The string returned by this method is the same as the string returned by the getRawPath method, except that all escaped octet sequences are decoded. |
| equals(ob: Object) | Test whether this URI is equal to another object. If the given object is not a URI, this method immediately returns false. |
| normalize​() | Normalize the path of this URI. If this URI is opaque, or its path is already in normal form, then this URI is returned. Otherwise, a new URI identical to this URI will be constructed. |
| checkIsAbsolute() | Determine whether this URI is absolute. If and only if it has a scheme component, the URI is absolute and the return value is true, otherwise the return value is false. |
| toString() | Return the content of this URI as a string. |
| ConvertXml() | The constructor used to construct the convertxml class object. This constructor does not need to pass in parameters. |
| convert(xml: string, options: Object)  | Returns a JavaScript object that converts an XML string as required by the option. |
| XmlSerializer(buffer: ArrayBuffer \| DataView, encoding?: string) | Create and return an XmlSerializer object. The XmlSerializer object passes two parameters. The first parameter is ArrayBuffer or DataView, and the second parameter is the file format (UTF-8 by default). |
| setAttributes(name: string, value: string): void | Write the Attributes attribute to the xml file. |
| addEmptyElement(name: string): void | Write an empty element. |
| setDeclaration(): void | Set the Declaration to use the encoding to write the xml declaration. For example: <? xml version="1.0" encoding="utf-8"> |
| startElement(name: string): void | Write the elemnet start tag with the given name.|
| endElement(): void | Write the end tag of the element. |
| setNamespace(prefix: string, namespace: string): void | Write the namespace of the current element tag. |
| setCommnet(text: string): void | Write the comment attribute. |
| setCData(text: string): void | Write the CData attribute. |
| setText(text: string): void | Write the Text property. |
| setDocType(text: string): void | Write the DocType attribute. |
| XmlPullParser(buffer: ArrayBuffer \| DataView, encoding?: string) | Create and return an XmlPullParser object. The XmlPullParser object passes two parameters. The first parameter is ArrayBuffer or DataView, and the second parameter is the file format (default is UTF-8). |
| parse(option: ParseOptions): void | This interface is used to parse xml. The ParseOptions parameter is an interface containing five optional parameters {supportDoctype?: boolea ignoreNameSpace?: boolean tagValueCallbackFunction?: (name: string, value: string) => boolean attributeValueCallbackFunction?: (name: string, value: string) => boolean) tokenValueCallbackFunction?: (eventType: EventType, value: ParseInfo) => boolean }. The input parameter 1 of the tokenValueCallbackFunction callback function is the event type, and the input parameter 2 is the info interface containing get attributes such as getColumnNumber and getDepth. The user can obtain the depth and other information in the current parsing process through methods such as info.getDepth(). |
#### 1.3.2.Instructions for use

The usage of each interface is as follows:


1、new URL(url: string,base?:string|URL)
```
let b = new URL('https://developer.mozilla.org'); // => 'https://developer.mozilla.org/'

let a = new URL( 'sca/./path/path/../scasa/text', 'http://www.example.com');
// => 'http://www.example.com/sca/path/scasa/text'
```
2、tostring():string
```
const url = new URL('http://10.0xFF.O400.235:8080/directory/file?query#fragment');
url.toString() // => 'http://10.0xff.o400.235:8080/directory/file?query#fragment'

const url = new URL("http://[FEDC:BA98:7654:3210:FEDC:BA98:7654:3210]:80/index.html");
url.toString() // => 'http://[fedc:ba98:7654:3210:fedc:ba98:7654:3210]/index.html'

const url = new URL("http://username:password@host:8080/directory/file?query#fragment");
url.toString() // => 'http://username:password@host:8080/directory/file?query#fragment'
```
3、toJSON():string
```
const url = new URL("https://developer.mozilla.org/en-US/docs/Web/API/URL/toString");
url.toJSON(); // =>  'https://developer.mozilla.org/en-US/docs/Web/API/URL/toString'
```
4、new URLSearchParams()
```
let params = new URLSearchParams('foo=1&bar=2');
```
5、new URLSearchParams(string)
```
params = new URLSearchParams('user=abc&query=xyz');
console.log(params.get('user'));
// Prints 'abc'
```
6、new URLSearchParams(obj)
```
const params = new URLSearchParams({
    user: 'abc',
    query: ['first', 'second']
});
console.log(params.getAll('query'));
// Prints [ 'first,second' ]
```
7、new URLSearchParams(iterable)
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
8、has(name: string): boolean
```
console.log(params.has('bar')); // =>true
```
9、set(name: string, value string): void
```
params.set('baz', 3);
```
10、sort(): void
```
params .sort();
```
11、toString(): string
```
console.log(params .toString()); // =>bar=2&baz=3&foo=1'
```
12、keys(): iterableIterator\<string>
```
for(var key of params.keys()) {
  console.log(key);
} // =>bar  baz  foo
```
13、values(): iterableIterator\<string>
```
for(var value of params.values()) {
  console.log(value);
} // =>2  3  1
```
14、append(name: string, value: string): void
```
params.append('foo', 3); // =>bar=2&baz=3&foo=1&foo=3
```
15、delete(name: string): void
```
params.delete('baz'); // => bar=2&foo=1&foo=3
```
16、get(name: string): string
```
params.get('foo'); // => 1
```
17、getAll(name: string): string[]
```
params.getAll('foo'); // =>[ '1', '3' ]
```
18、entries(): iterableIterator<[string, string]>
```
for(var pair of searchParams.entries()) {
   console.log(pair[0]+ ', '+ pair[1]);
} // => bar, 2   foo, 1  foo, 3
```
19、forEach(): void
```
url.searchParams.forEach((value, name, searchParams) => {
  console.log(name, value, url.searchParams === searchParams);
});
// => foo 1 true
// => bar 2 true
```
20、urlSearchParams[Symbol.iterator] ()
```
const params = new URLSearchParams('foo=bar&xyz=baz');
for (const [name, value] of params) {
    console.log(name, value);
}
// Prints:
// foo bar
// xyz bar
```


21、URI​(String str)
```
let gaogao = new Uri.URI('http://gg:gaogao@www.baidu.com:99/path/path?query#fagment');
```
22、scheme
```
let gaogao = new Uri.URI('http://gg:gaogao@www.baidu.com:99/path/path?query#fagment');
gaogao.scheme        // => "http";
```
23、authority
```
let gaogao = new Uri.URI('http://gg:gaogao@www.baidu.com:99/path/path?query#fagment');
gaogao.authority     // => "gg:gaogao@www.baidu.com:99";
```
24、ssp
```
let gaogao = new Uri.URI('http://gg:gaogao@www.baidu.com:99/path/path?query#fagment');
gaogao.ssp "         // => gg:gaogao@www.baidu.com:99/path/path?query";
```
25、userinfo
```
let gaogao = new Uri.URI('http://gg:gaogao@www.baidu.com:99/path/path?query#fagment');
gaogao.userinfo      // => "gg:gaogao";
```
26、host
```
let gaogao = new Uri.URI('http://gg:gaogao@www.baidu.com:99/path/path?query#fagment');
gaogao.host          // => "www.baidu.com";
```
27、port
```
let gaogao = new Uri.URI('http://gg:gaogao@www.baidu.com:99/path/path?query#fagment');
gaogao.port          // => "99";
```
28、query
```
let gaogao = new Uri.URI('http://gg:gaogao@www.baidu.com:99/path/path?query#fagment');
gaogao.query         // => "query";
```
29、fragment
```
let gaogao = new Uri.URI('http://gg:gaogao@www.baidu.com:99/path/path?query#fagment');
gaogao.fragment      // => "fagment";
```
30、path
```
let gaogao = new Uri.URI('http://gg:gaogao@www.baidu.com:99/path/path?query#fagment');
gaogao.path          // => "/path/path";
```
31、equals(Object ob)
```
let gaogao = new Uri.URI('http://gg:gaogao@[1:0:0:1:2:1:2:1]:99/path1?query#fagment');
let gaogao1 = gaogao;
let res = gaogao.equals(gaogao1);
console.log(res);      // => true;
```
32、normalize​()
```
let gaogao = new Uri.URI('http://gg:gaogao@[1:0:0:1:2:1:2:1]:99/path/66./../././mm/.././path1?query#fagment');
let res = gaogao.normalize();
console.log(res.path);        // => "/path/path1"
console.log(res.toString());  // => "http://gg:gaogao@[1:0:0:1:2:1:2:1]:99/path/path1?query#fagment"
```
33、checkIsAbsolute()
```
let gaogao = new Uri.URI('f/tp://username:password@www.baidu.com:88/path?query#fagment');
let res = gaogao.checkIsAbsolute();
console.log(res);              //=> false;
```
34、toString()
```
let gaogao = new Uri.URI('http://gg:gaogao@[1:0:0:1:2:1:2:1]:99/../../path/.././../aa/bb/cc?query#fagment');
let res = gaogao.toString();
console.log(res.toString());     // => 'http://gg:gaogao@[1:0:0:1:2:1:2:1]:99/../../path/.././../aa/bb/cc?query#fagment';
```


35、ConvertXml()
```
var convertml = new convertXml.ConvertXml();
```
36、convert(xml: string, options: Object)
```
var result = convertml.convert(xml, {compact: false, spaces: 4});
```
37、new XmlSerializer(buffer: ArrayBuffer | DataView, encoding?: string)
```

var arrayBuffer = new ArrayBuffer(1024);
var bufView = new DataView(arrayBuffer);
var thatSer = new xml.XmlSerializer(bufView);
```
38、setDeclaration()：void
```
var thatSer = new xml.XmlSerializer(bufView);
thatSer.setDeclaration() // => <?xml version="1.0" encoding="utf-8"?>;
```
39、setCommnet(text: string):void
```
var thatSer = new xml.XmlSerializer(bufView);
thatSer.setCommnet("Hello, World!"); // => <!--Hello, World!-->;
```
40、setCData(text: string) :void
```
var thatSer = new xml.XmlSerializer(bufView);
thatSer.setDocType('root SYSTEM "http://www.test.org/test.dtd"'); // => <![CDATA[root SYSTEM \“http://www.test.org/test.dtd\”]]>
```
41、setDocType(text: string):void
```
var thatSer = new xml.XmlSerializer(bufView);
thatSer.setDocType("foo"); // => <!DOCTYPE foo>
```
42、setNamespace(prefix: string, namespace: string): void
43、startElement(name: string): void
44、setAttributes(name: string, value: string): void
45、endElement(): void
46、setText(text: string): void
```
var thatSer = new xml.XmlSerializer(bufView);
thatSer.setNamespace("h", "http://www.w3.org/TR/html4/");
thatSer.startElement("table");
thatSer.setAttributes("importance", "high");
thatSer.setText("Happy");
endElement(); // => <h:table importance="high" xmlns:h="http://www.w3.org/TR/html4/">Happy</h:table>
```
47、addEmptyElement(name: string): void
```
var thatSer = new xml.XmlSerializer(bufView);
thatSer.addEmptyElement("b"); // => <b/>
```
48、new (buffer: ArrayBuffer | DataView, encoding?: string)
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
    bufView[i] = strXml.charCodeAt(i);//设置arraybuffer 方式
}
var that = new xml.XmlPullParser(arrayBuffer);

```
49、parse(option: ParseOptions): void
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
var options = {supportDoctype:true, ignoreNameSpace:true, tokenValueCallbackFunction:func};
that.parse(options);
```

## 2.js_util_module submodule
### 2.1.Introduction
The interface of util is used for character Textencoder, TextDecoder and HelpFunction module.The TextEncoder represents a text encoder that accepts a string as input, encodes it in UTF-8 format, and outputs UTF-8 byte stream. The TextDecoder interface represents a text decoder. The decoder takes the byte stream as the input and outputs the String string. HelpFunction is mainly used to callback and promise functions, write and output error codes, and format class strings.
Encodes all bytes from the specified u8 array into a newly-allocated u8 array using the Base64 encoding scheme or Encodes the specified byte array into a String using the Base64 encoding scheme.Decodes a Base64 encoded String or input u8 array into a newly-allocated u8 array using the Base64 encoding scheme.The rational number is mainly to compare rational numbers and obtain the numerator and denominator.The LruBuffer algorithm replaces the least used data with new data when the buffer space is insufficient. The algorithm derives from the need to access resources: recently accessed data can be Will visit again in the near future. The least accessed data is the least valuable data that should be kicked out of the cache space. The Scope interface is used to describe the valid range of a field. The constructor for the Scope instance is used to create objects with specified lower and upper bounds and require that these objects be comparable.
### 2.2.contants

```
commomlibrary/ets_utils/js_util_module/
├── Class:TextEncoder                   # TextEncoder class
│   ├──  new TextEncoder()              # create textencoder object
│   ├──  encode()                       # encode method
│   ├──  encoding                       # encoding property
│   └──  encodeInto()                   # encodeInto method
├── Class:TextDecoder                   # TextDecoder class
│   ├──  new TextDecoder()              # create TextDecoder object
│   ├──  decode()                       # decode method
|   ├──  decodeWithStream()             # decodeWithStream method
│   ├──  encoding                       # encoding property
│   ├──  fatal                          # fatal property
│   └──  ignoreBOM                      # ignoreBOM property
├── printf()                            # printf method
├── getErrorString()                    # getErrorString method
├── callbackWrapper()                   # callbackWrapper method
├── promiseWrapper()                    # promiseWrapper method
├── Class:Base64                        # Base64 class
│   ├──  new Base64()                   # create Base64 object
│   ├──  encodeSync()                   # encodeSync method
│   ├──  encodeToStringSync()           # encodeToStringSync method
│   ├──  decodeSync()                   # decodeSync method
│   ├──  encode()                       # encode method
│   ├──  encodeToString()               # encodeToString method
│   └──  decode()                       # decode method
├── Class:RationalNumber                # RationalNumber class
│   ├──  new RationalNumber()           # create RationalNumber object
│   ├──  createRationalFromString()     # creatRationalFromString method
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
│   ├──  new LruBuffer()                # create RationalNumber object
│   ├──  updateCapacity()               # updateCapacity method
│   ├──  toString()                     # toString method
│   ├──  values()                       # values method
│   ├──  length                         # attribute of length
│   ├──  getCapacity()                  # getCapacity method
│   ├──  clear()                        # clear method
│   ├──  getCreateCount                 # getCreateCount method
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
├── Class:Scope                         # Scope class
|   ├── constructor()                   # create Scope object
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
    ├── isFloat32Array()                # isFloat32Arraymethod
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

### 2.3.Description

#### 2.3.1.Interface description
| Interface name | Description |
| -------- | -------- |
| constructor(encoding? : string) | Constructor, the parameter encoding indicates the format of encoding. Default utf-8, Support gb18030, gbk, gb2312. |
| readonly encoding : string | In the TextEncoder module, get the encoding format. |
| encode(input : string) : Uint8Array | Input string string, encoding according to encoding and output uint8 byte stream. |
| encodeInto(input : string, dest : Uint8Array) : {read : number, written : number} | Enter the string string, dest represents the storage location after encoding, and returns an object, read represents the number of characters that have been encoded,and written represents the size of bytes occupied by the encoded characters. |
| constructor(encoding? : string, options? : {fatal? : boolean, ignoreBOM? : boolean}) | Constructor, the first parameter encoding indicates the format of decoding.The second parameter represents some attributes.Fatal in the attribute indicates whether an exception is thrown, and ignoreBOM indicates whether to ignore the bom flag. |
| readonly encoding : string | In the TextDecoder module, get the set decoding format. |
| readonly fatal : boolean | Get the setting that throws the exception. |
| readonly ignoreBOM : boolean | Get whether to ignore the setting of the bom flag. |
| decode(input : Uint8Array, options?: { stream?: false }) : string | Input the data to be decoded, and solve the corresponding string character string.The first parameter input represents the data to be decoded, and the second parameter options represents a bool flag, which means that additional data will be followed. The default is false. |
| decodeWithStream(input : Uint8Array, options?: { stream?: false }) : string | Input the data to be decoded, and solve the corresponding string character string.The first parameter input represents the data to be decoded, and the second parameter options represents a bool flag, which means that additional data will be followed. The default is false. |
| encodeSync(src: Uint8Array): Uint8Array; | Encodes all bytes in the specified u8 array into the newly allocated u8 array using the Base64 encoding scheme. |
| encodeToStringSync(src: Uint8Array): string; | Encodes the specified byte array as a String using the Base64 encoding scheme. |
| decodeSync(src: Uint8Array \| string): Uint8Array; | Decodes the Base64-encoded string or input u8 array into the newly allocated u8 array using the Base64 encoding scheme. |
| encode(src: Uint8Array): Promise\<Uint8Array\>; | Asynchronously encodes all bytes in the specified u8 array into the newly allocated u8 array using the Base64 encoding scheme. |
| encodeToString(src: Uint8Array): Promise\<string\>; | Asynchronously encodes the specified byte array into a String using the Base64 encoding scheme. |
| decode(src: Uint8Array \| string): Promise\<Uint8Array\>; | Use the Base64 encoding scheme to asynchronously decode a Base64-encoded string or input u8 array into a newly allocated u8 array. |
| static createRationalFromString(rationalString: string): RationalNumber | Create a RationalNumber object based on the given string. |
| compareTo(another: RationalNumber): number | Compare the current RationalNumber object with the given object. |
| equals(obj: object): number | Check if the given object is the same as the current RationalNumber object.|
| valueOf(): number | Take the current RationalNumber object to an integer value or a floating point value. |
| static getCommonDivisor(number1: number, number2: number,): number | Obtain the greatest common divisor of two specified numbers. |
| getDenominator(): number | Get the denominator of the current RationalNumber object. |
| getNumerator(): number | Get the numerator of the current RationalNumber object. |
| isFinite(): boolean | Check that the current RationalNumber object is limited. |
| isNaN(): boolean | Check whether the current RationalNumber object represents a non-number (NaN) value. |
| isZero(): boolean | Check whether the current RationalNumber object represents a zero value. |
| toString(): string | Get the string representation of the current RationalNumber object. |
| constructor(capacity?: number) | The Create Default constructor is used to create a new LruBuffer instance with a default capacity of 64. |
| updateCapacity(newCapacity: number): void | Updates the buffer capacity to the specified capacity. This exception is thrown if newCapacity is less than or equal to 0. |
| toString(): string | Returns the string representation of the object and outputs the string representation of the object. |
| values(): V[ ] | Gets a list of all values in the current buffer, and the output returns a list of all values in the current buffer in ascending order, from most recently accessed to least recently accessed. |
| length: number | represents the total number of values in the current buffer. The output returns the total number of values in the current buffer. |
| getCapacity(): number | Gets the capacity of the current buffer. The output returns the capacity of the current buffer. |
| clear(): void | The key value pairs are cleared from the current buffer, after the key value is cleared, the afterRemoval () method is invoked to perform subsequent operations in turn. |
| getCreateCount(): number | Get the number of times the returned value of createdefault(), and output the number of times the returned value of createdefault(). |
| getMissCount(): number | Get the number of times the query value does not match, and output the number of times the query value does not match. |
| getRemovalCount(): number | Gets the number of evictions from the buffer, and outputs the number of evictions from the buffer. |
| getMatchCount​(): number | Obtain the number of successful matching of query values, and output the number of successful matching of query values. |
| getPutCount(): number | Gets the number of times the value was added to the buffer, and the output returns the number of times the value was added to the buffer. |
| isEmpty(): boolean | Checks whether the current buffer is empty and returns true if the current buffer does not contain any values. |
| get(key: K):V \| undefined | Indicates the key to query. If the specified key exists in the buffer, the value associated with the key will be returned; Otherwise, undefined is returned. |
| put(key: K, value: V): V | Adding the key value pair to the buffer and outputting the value associated with the added key; If the key to be added already exists, the original value is returned. If the key or value is empty, this exception is thrown. |
| keys(): K[ ] | Get the key list of the value in the current buffer, and the output returns the key list sorted from the most recent access to the least recent access. |
| remove​(key: K):V \| undefined | Deletes the specified key and its associated value from the current buffer. |
| afterRemoval(isEvict: boolean, key: K, value: V, newValue: V): void | Perform subsequent operations after deleting the value. |
| contains(key: K): boolean | Checks whether the current buffer contains the specified key, and returns true if the buffer contains the specified key. |
| createDefault(key: K): V | If the value of a specific key is not calculated, subsequent operations are performed. The parameter represents the missing key, and the output returns the value associated with the key. |
| entries(): [K,V] | Allows you to iterate over all key value pairs contained in this object. The keys and values of each pair are objects. |
| \[Symbol.iterator\](): [K,V]| Returns a two-dimensional array in the form of key value pairs. |
| constructor(lowerObj: ScopeType, upperObj: ScopeType) | Creates and returns a Scope object that creates a constructor for a scope instance that specifies a lower and upper bound. |
| toString():string | The stringification method returns a  string representation that contains the current range. |
| intersect(range: Scope): Scope | Gets the intersection of the given range and the current range. |
| intersect(lowerObj: ScopeType, upperObj: ScopeType): Scope | Gets the intersection of the current range with a given lower and upper bound range. |
| getUpper(): ScopeType | Gets the upper bound of the current scope. |
| getLower(): ScopeType | Gets the lower bound of the current scope. |
| expand(lowerObj: ScopeType, upperObj:  ScopeType): Scope | Creates and returns a union that includes the current range and a given lower and upper bound. |
| expand(range: Scope): Scope | Creates and returns a union that includes the current range and the given range. |
| expand(value: ScopeType): Scope | Creates and returns a union that includes the current range and the given value. |
| contains(value: ScopeType): boolean | Checks whether the given value is included in the current range. |
| contains(range: Scope): boolean | Checks whether the given range is within the current range. |
| clamp(value: ScopeType): ScopeType | Clips the specified value to the current range. |
| function printf(format: string, ...args: Object[]): string | The util.format() method returns a formatted string using the first argument as a printf-like format string which can contain zero or more format specifiers. |
| function getErrorString(errno: number): string |  The geterrorstring () method uses a system error number as a parameter to return system error information. |
| function callbackWrapper(original: Function): (err: Object, value: Object) => void | Takes an async function (or a function that returns a Promise) and returns a function following the error-first callback style, i.e. taking an (err, value) => ... callback as the last argument. In the callback, the first argument will be the rejection reason (or null if the Promise resolved), and the second argument will be the resolved value. |
| function promiseWrapper(original: (err: Object, value: Object) => void): Object | Takes a function following the common error-first callback style, i.e. taking an (err, value) => ... callback as the last argument, and returns a version that returns promises. |
| isAnyArrayBuffer(value: Object): boolean | Check whether the entered value is of arraybuffer or sharedarraybuffer type. |
| isArrayBufferView(value: Object): boolean | Check whether the entered value is napi_ int8_ array or napi_ uint8_ array or naPi_ uint8_ clamped_ array or naPi_ int16_ array or naPi_ uint16_ array or napi_ int32_ array or napi_ uint32_ array or napi_ float32_ array or napi_ float64_ array array or DataView type. |
| isArgumentsObject(value: Object): boolean | Check whether the entered value is an arguments object type. |
| isArrayBuffer(value: Object): boolean | Check whether the entered value is of arraybuffer type. |
| isAsyncFunction(value: Object): boolean | Check whether the value entered is an asynchronous function type. |
| isBigInt64Array(value: Object): boolean | Check whether the entered value is of bigint64array array type. |
| isBigUint64Array(value: Object): boolean | Check whether the entered value is of biguint64array array array type. |
| isBooleanObject(value: Object): boolean | Check whether the entered value is a Boolean object type. |
| isBoxedPrimitive(value: Object): boolean | Check whether the entered value is a Boolean or number or string or symbol object type. |
| isDataView(value: Object): boolean | Check whether the entered value is of DataView type. |
| isDate(value: Object): boolean | Check whether the entered value is of type date. |
| isExternal(value: Object): boolean | Check whether the entered value is a native external value type. |
| isFloat32Array(value: Object): boolean | Check whether the entered value is of float32array array type. |
| isFloat64Array(value: Object): boolean | Check whether the entered value is of float64array array type. |
| isGeneratorFunction(value: Object): boolean | Check whether the input value is a generator function type. |
| isGeneratorObject(value: Object): boolean | Check whether the entered value is a generator object type. |
| isInt8Array(value: Object): boolean | Check whether the entered value is of int8array array type. |
| isInt16Array(value: Object): boolean | Check whether the entered value is the int16array type. |
| isInt32Array(value: Object): boolean | Check whether the entered value is the int32array array type. |
| isMap(value: Object): boolean | Check whether the entered value is of map type. |
| isMapIterator(value: Object): boolean | Check whether the entered value is the iterator type of map. |
| isModuleNamespaceObject(value: Object): boolean | Check whether the entered value is the module namespace object object type. |
| isNativeError(value: Object): boolean | Check whether the value entered is of type error. |
| isNumberObject(value: Object): boolean | Check whether the entered value is of the number object type. |
| isPromise(value: Object): boolean | Check whether the entered value is of promise type. |
| isProxy(value: Object): boolean | Check whether the value entered is of proxy type. |
| isRegExp(value: Object): boolean | Check whether the entered value is of type regexp. |
| isSet(value: Object): boolean | Check whether the entered value is of type set. |
| isSetIterator(value: Object): boolean | Check whether the entered value is the iterator type of set. |
| isSharedArrayBuffer(value: Object): boolean | Check whether the entered value is of type sharedarraybuffer. |
| isStringObject(value: Object): boolean | Check whether the entered value is a string object type. |
| isSymbolObject(value: Object): boolean | Check whether the entered value is a symbol object type. |
| isTypedArray(value: Object): boolean | Check whether the entered value is a type contained in typedarray. |
| isUint8Array(value: Object): boolean | Check whether the entered value is the uint8array array type. |
| isUint8ClampedArray(value: Object): boolean | Check whether the entered value is the uint8clapedarray array type. |
| isUint16Array(value: Object): boolean | Check whether the entered value is the uint16array array array type. |
| isUint32Array(value: Object): boolean | Check whether the entered value is the uint32array array type. |
| isWeakMap(value: Object): boolean | Check whether the entered value is of type weakmap. |
| isWeakSet(value: Object): boolean | Check whether the entered value is of type weakset. |

Each specifier in printf is replaced with a converted value from the corresponding parameter. Supported specifiers are:
| Stylized character | Style requirements |
| -------- | -------- |
|    %s:  | String will be used to convert all values except BigInt, Object and -0. |
|    %d:  | Number will be used to convert all values except BigInt and Symbol. |
|    %i:  | parseInt(value, 10) is used for all values except BigInt and Symbol. |
|    %f:  | parseFloat(value) is used for all values expect Symbol. |
|    %j:  | JSON. Replaced with the string '[Circular]' if the argument contains circular references. |
|    %o:  | Object. A string representation of an object with generic JavaScript object formatting. Similar to util.inspect() with options { showHidden: true, showProxy: true }. This will show the full object including non-enumerable properties and proxies. |
|    %O:  | Object. A string representation of an object with generic JavaScript object formatting. Similar to util.inspect() without options. This will show the full object not including non-enumerable properties and proxies. |
|    %c:  | CSS. This specifier is ignored and will skip any CSS passed in. |
|    %%:  | single percent sign ('%'). This does not consume an argument. |

#### 2.3.2.Instruction for use


The use methods of each interface are as follows:

1.readonly encoding()

```
import util from '@ohos.util'
var textEncoder = new util.TextEncoder();
var getEncoding = textEncoder.encoding();
```
2.encode()
```
import util from '@ohos.util'
var textEncoder = new util.TextEncoder();
var result = textEncoder.encode('abc');
```
3.encodeInto()
```
import util from '@ohos.util'
var textEncoder = new util.TextEncoder();
var obj = textEncoder.encodeInto('abc', dest);
```
4.textDecoder()
```
import util from '@ohos.util'
var textDecoder = new util.textDecoder("utf-16be", {fatal : true, ignoreBOM : false});
```
5.readonly encoding()
```
import util from '@ohos.util'
var textDecoder = new util.textDecoder("utf-16be", {fatal : true, ignoreBOM : false});
var getEncoding = textDecoder.encoding();
```
6.readonly fatal()
```
import util from '@ohos.util'
var textDecoder = new util.textDecoder("utf-16be", {fatal : true, ignoreBOM : false});
var fatalStr = textDecoder.fatal();
```
7.readonly ignoreBOM()
```
import util from '@ohos.util'
var textDecoder = new util.textDecoder("utf-16be", {fatal : true, ignoreBOM : false});
var ignoreBom = textDecoder.ignoreBOM();
```
8.decode()
```
import util from '@ohos.util'
var textDecoder = new util.textDecoder("utf-16be", {fatal : true, ignoreBOM : false});
var result = textDecoder.decode(input, {stream : true});
```
9.decodeWithStream()
```
import util from '@ohos.util'
var textDecoder = new util.textDecoder("utf-16be", {fatal : true, ignoreBOM : false});
var result = textDecoder.decodeWithStream(input, {stream : true});
```
10.printf()
```
import util from '@ohos.util'
var format = "%%%o%%%i%s";
var value =  function aa(){};
var value1 = 1.5;
var value2 = "qwer";
var result = util.printf(format,value,value1,value2);
```
11.getErrorString()
```
import util from '@ohos.util'
var errnum = 13;
var result = util.getErrorString(errnum);
```
12.callbackWrapper()
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
13.promiseWrapper()
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
14.encodeSync()
```
import util from '@ohos.util'
var that = new util.Base64();
var array = new Uint8Array([115,49,51]);
var result = that.encodeSync(array);
```
15.encodeToStringSync()
```
import util from '@ohos.util'
var that = new util.Base64();
var array = new Uint8Array([115,49,51]);
var result = that.encodeToStringSync(array);
```
16.decodeSync()
```
import util from '@ohos.util'
var that = new util.Base64()
var buff = 'czEz';
var result = that.decodeSync(buff);
```
17.encode()
```
import util from '@ohos.util'
var that = new util.Base64()
var array = new Uint8Array([115,49,51]);
await that.encode(array).then(val=>{
})
done()
```
18.encodeToString()
```
import util from '@ohos.util'
var that = new util.Base64()
var array = new Uint8Array([115,49,51]);
await that.encodeToString(array).then(val=>{
})
done()
```
19.decode()
```
import util from '@ohos.util'
var that = new util.Base64()
var buff = 'czEz';
await that.decode(buff).then(val=>{
})
done()
```
20.createRationalFromString()
```
import util from '@ohos.util'
var pro = new util.RationalNumber(0, 0);
var res = pro.createRationalFromString("-1:2");
var result1 = res.value();
```
21.compareTo()
```
import util from '@ohos.util'
var pro = new util.RationalNumber(2, 1);
var proc = new util.RationalNumber(3, 4);
var res = pro.compareTo(proc);
```
22.equals()
```
import util from '@ohos.util'
var pro = new util.RationalNumber(2, 1);
var proc = new util.RationalNumber(3, 4);
var res = pro.equals(proc);
```
23.valueOf()
```
import util from '@ohos.util'
var pro = new util.RationalNumber(2, 1);
var res = pro.value();
```
24.getCommonDivisor()
```
import util from '@ohos.util'
var pro = new util.RationalNumber(0, 0);
var res = pro.getCommonDivisor(4, 8);
```
25.getDenominator()
```
import util from '@ohos.util'
var pro = new util.RationalNumber(2, 1);
var res = pro.getDenominator();
```
26.getNumerator()
```
import util from '@ohos.util'
var pro = new util.RationalNumber(-2, 1);
var res = pro.getNumerator();
```
27.isFinite()
```
import util from '@ohos.util'
var pro = new util.RationalNumber(-2, 1);
var res = pro.isFinite();
```
28.isNaN()
```
import util from '@ohos.util'
var pro = new util.RationalNumber(-2, 1);
var res = pro.isNaN();
```
29.isZero()
```
import util from '@ohos.util'
var pro = new util.RationalNumber(-2, 1);
var res = pro.isZero();

```
30.toString()
```
import util from '@ohos.util'
var pro = new util.RationalNumber(-2, 1);
var res = pro.toString();

```
31.updateCapacity()
```
import util from '@ohos.util'
var pro = new util.LruBuffer();
var result = pro.updateCapacity(100);
```
32.toString()
```
import util from '@ohos.util'
var pro = new util.LruBuffer();
pro.put(2,10);
pro.get(2);
pro.remove(20);
var result = pro.toString();
```
33.values()
```
import util from '@ohos.util'
var pro = new util.LruBuffer();
pro.put(2,10);
pro.put(2,"anhu");
pro.put("afaf","grfb");
var result = pro.values();
```
34.length
```
import util from '@ohos.util'
var pro = new util.LruBuffer();
pro.put(2,10);
pro.put(1,8);
var result = pro.length;
```
35.getCapacity()
```
import util from '@ohos.util'
var pro = new util.LruBuffer();
var result = pro.getCapacity();
```
36.clear()
```
import util from '@ohos.util'
var pro = new util.LruBuffer();
pro.put(2,10);
pro.clear();
```
37.getCreateCount()
```
import util from '@ohos.util'
var pro = new util.LruBuffer();
pro.put(1,8);
var result = pro.getCreateCount();
```
38.getMissCount()
```
import util from '@ohos.util'
var pro = new util.LruBuffer();
pro.put(2,10);
pro.get(2)
var result = pro.getMissCount();
```
39.getRemovalCount()
```
import util from '@ohos.util'
var pro = new util.LruBuffer();
pro.put(2,10);
pro.updateCapacity(2);
pro.put(50,22);
var result = pro.getRemovalCount();
```
40.getMatchCount()
```
import util from '@ohos.util'
var pro = new util.LruBuffer();
pro.put(2,10);
pro.get(2);
var result = pro.getMatchCount();
```
41.getPutCount()
```
import util from '@ohos.util'
var pro = new util.LruBuffer();
pro.put(2,10);
var result = pro.getPutCount();
```
42.isEmpty()
```
import util from '@ohos.util'
var pro = new util.LruBuffer();
pro.put(2,10);
var result = pro.isEmpty();
```
43.get()
```
import util from '@ohos.util'
var pro = new util.LruBuffer();
pro.put(2,10);
var result = pro.get(2);
```
44.put()
```
import util from '@ohos.util'
var pro = new util.LruBuffer();
var result = pro.put(2,10);
```
45.keys()
```
import util from '@ohos.util'
var pro = new util.LruBuffer();
 pro.put(2,10);
var result = pro.keys();
```
46.remove()
```
import util from '@ohos.util'
var pro = new util.LruBuffer();
pro.put(2,10);
var result = pro.remove(20);
```
47.contains()
```
import util from '@ohos.util'
var pro = new util.LruBuffer();
pro.put(2,10);
var result = pro.contains(20);
```
48.createDefault()
```
import util from '@ohos.util'
var pro = new util.LruBuffer();
var result = pro.createDefault(50);
```
49.entries()
```
import util from '@ohos.util'
var pro = new util.LruBuffer();
pro.put(2,10);
var result = pro.entries();
```
50.\[Symbol.iterator\]()
```
import util from '@ohos.util'
var pro = new util.LruBuffer();
pro.put(2,10);
var result = pro[symbol.iterator]();
```
51.afterRemoval()
```
import util from '@ohos.util'
var arr = [];
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
Construct a new class in the Scope interface to implement the compareTo method.

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

52.constructor()

```
var tempLower = new Temperature(30);
var tempUpper = new Temperature(40);
var range = new Scope(tempLower, tempUpper);
```

53.toString()

```
var tempLower = new Temperature(30);
var tempUpper = new Temperature(40);
var range = new Scope(tempLower, tempUpper);
var result = range.toString() // => [30,40]
```

54.intersect()

```
var tempLower = new Temperature(30);
var tempUpper = new Temperature(40);
var range = new Scope(tempLower, tempUpper);
var tempMiDF = new Temperature(35);
var tempMidS = new Temperature(39);
var rangeFir = new Scope(tempMiDF, tempMidS);
var result = range.intersect(rangeFir)  // => [35,39]
```

55.intersect()

```
var tempLower = new Temperature(30);
var tempUpper = new Temperature(40);
var tempMiDF = new Temperature(35);
var tempMidS = new Temperature(39);
var range = new Scope(tempLower, tempUpper);
var result = range.intersect(tempMiDF, tempMidS)  // => [35,39]
```

56.getUpper()

```
var tempLower = new Temperature(30);
var tempUpper = new Temperature(40);
var range = new Scope(tempLower, tempUpper);
var result = range.getUpper() // => 40
```

57.getLower()

```
var tempLower = new Temperature(30);
var tempUpper = new Temperature(40);
var range = new Scope(tempLower, tempUpper);
var result = range.getLower() // => 30
```

58.expand()

```
var tempLower = new Temperature(30);
var tempUpper = new Temperature(40);
var tempMiDF = new Temperature(35);
var tempMidS = new Temperature(39);
var range = new Scope(tempLower, tempUpper);
var result = range.expand(tempMiDF, tempMidS)  // => [30,40]
```

59.expand()

```
var tempLower = new Temperature(30);
var tempUpper = new Temperature(40);
var tempMiDF = new Temperature(35);
var tempMidS = new Temperature(39);
var range = new Scope(tempLower, tempUpper);
var rangeFir = new Scope(tempMiDF, tempMidS);
var result = range.expand(rangeFir) // => [30,40]
```

60.expand()

```
var tempLower = new Temperature(30);
var tempUpper = new Temperature(40);
var tempMiDF = new Temperature(35);
var range = new Scope(tempLower, tempUpper);
var result = range.expand(tempMiDF)  // => [30,40]
```

61.contains()

```
var tempLower = new Temperature(30);
var tempUpper = new Temperature(40);
var tempMiDF = new Temperature(35);
var range = new Scope(tempLower, tempUpper);
var result = range.contains(tempMiDF) // => true
```

62.contains()

```
var tempLower = new Temperature(30);
var tempUpper = new Temperature(40);
var range = new Scope(tempLower, tempUpper);
var tempLess = new Temperature(20);
var tempMore = new Temperature(45);
var rangeSec = new Scope(tempLess, tempMore);
var result = range.contains(rangeSec) // => true
```

63.clamp()

```
var tempLower = new Temperature(30);
var tempUpper = new Temperature(40);
var tempMiDF = new Temperature(35);
var range = new Scope(tempLower, tempUpper);
var result = range.clamp(tempMiDF) // => 35
```
64.isAnyArrayBuffer()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isAnyArrayBuffer(new ArrayBuffer([]))
```
65.isArrayBufferView()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isArrayBufferView(new DataView(new ArrayBuffer(16)));
```
66.isArgumentsObject()
```
import util from '@ohos.util'
function foo() {
        var result = proc.isArgumentsObject(arguments);
    }
var f = foo();
```
67.isArrayBuffer()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isArrayBuffer(new ArrayBuffer([]));
```
68.isAsyncFunction()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isAsyncFunction(async function foo() {});
```
69.isBigInt64Array()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isBigInt64Array(new Int16Array([]));
```
70.isBigUint64Array()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isBigUint64Array(new Int16Array([]));
```
71.isBooleanObject()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isBooleanObject(new Boolean(false));
```
72.isBoxedPrimitive()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isBoxedPrimitive(new Boolean(false));
```
73.isDataView()
```
import util from '@ohos.util'
var proc = new util.Types();
const ab = new ArrayBuffer(20);
var result = proc.isDataView(new DataView(ab));
```
74.isDate()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isDate(new Date());
```
75.isExternal()
```
import util from '@ohos.util'
const data = util.createExternalType();
var reult13 = proc.isExternal(data);
```
76.isFloat32Array()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isFloat32Array(new Float32Array([]));
```
77.isFloat64Array()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isFloat64Array(new Float64Array([]));
```
78.isGeneratorFunction()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isGeneratorFunction(function* foo() {});
```
79.isGeneratorObject()
```
import util from '@ohos.util'
var proc = new util.Types();
function* foo() {}
const generator = foo();
var result = proc.isGeneratorObject(generator);
```
80.isInt8Array()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isInt8Array(new Int8Array([]));
```
81.isInt16Array()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isInt16Array(new Int16Array([]));
```
82.isInt32Array()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isInt32Array(new Int32Array([]));
```
83.isMap()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isMap(new Map());
```
84.isMapIterator()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isMapIterator(map.keys());
```
85.isModuleNamespaceObject()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isModuleNamespaceObject(util);
```
86.isNativeError()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isNativeError(new TypeError());
```
87.isNumberObject()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isNumberObject(new Number(0));
```
88.isPromise()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isPromise(Promise.resolve(42));
```
89.isProxy()
```
import util from '@ohos.util'
var proc = new util.Types();
const target = {};
const proxy = new Proxy(target, {});
var result = proc.isProxy(proxy);
```
90.isRegExp()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isRegExp(new RegExp('abc'));
```
91.isSet()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isSet(new Set());
```
92.isSetIterator()
```
import util from '@ohos.util'
var proc = new util.Types();
const set = new Set();
var result = proc.isSetIterator(set.keys());
```
93.isSharedArrayBuffer()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isSharedArrayBuffer(new ArrayBuffer([]));
```
94.isStringObject()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isStringObject(new String('foo'));
```
95.isSymbolObject()
```
import util from '@ohos.util'
var proc = new util.Types();
const symbols = Symbol('foo');
var result = proc.isSymbolObject(Object(symbols));
```
96.isTypedArray()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isTypedArray(new Float64Array([]));
```
97.isUint8Array()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isUint8Array(new Uint8Array([]));
```
98.isUint8ClampedArray()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isUint8ClampedArray(new Uint8ClampedArray([]));
```
99.isUint16Array()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isUint16Array(new Uint16Array([]));
```
100.isUint32Array()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isUint32Array(new Uint32Array([]));
```
101.isWeakMap()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isWeakMap(new WeakMap());
```
102.isWeakSet()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isWeakSet(new WeakSet());
```

## 3.js_sys_module submodule
### 3.1.Introduction
Process is mainly used to obtain the relevant ID of the process, obtain and modify the working directory of the process, exit and close the process. The childprocess object can be used to create a new process. The main process can obtain the standard input and output of the child process, send signals and close the child process. 
### 3.2.Directory

```
commomlibrary/ets_utils/js_sys_module/
├── Class:PROCESS                   # class of PROCESS
├── Uid                             # attribute of Uid
├── Gid                             # attribute of Gid
├── EUid                            # attribute of EUid
├── EGid                            # attribute of EGid
├── Groups                          # attribute of Groups
├── Pid                             # attribute of Pid
├── Ppid                            # attribute of Ppid
├── chdir()                         # method of chdir
├── uptime()                        # method of uptime
├── kill()                          # method of kill
├── abort()                         # method of abort
├── on()                            # method of on
├── tid                             # method of tid
├── getStartRealtime()              # method of getStartRealtime
├── getAvailableCores()             # method of getAvailableCores
├── getPastCputime()                # method of getPastCputime
├── isIsolatedProcess()             # method of isIsolatedProcess
├── is64Bit()                       # method of is64Bit
├── isAppUid()                      # method of isAppUid
├── getUidForName()                 # method of getUidForName
├── getThreadPriority()             # method of getThreadPriority
├── getSystemConfig()               # method of getSystemConfig
├── getEnvironmentVar()             # method of getEnvironmentVar
├── exit()                          # method of exit
├── cwd()                           # method of cwd
├── off()                           # method of off
├── runCmd()                        # method of runCmd
└─── Class:CHILDPROCESS             # class of CHILDPROCESS
    ├── close()                     # method of close
    ├── kill()                      # method of kill
    ├── getOutput()                 # method of getOutput
    ├── getErrorOutput()            # method of getErrorOutput
    ├── wait()                      # method of wait
    ├── killed                      # attribute of killed
    ├── pid                         # attribute of pid
    ├── ppid                        # attribute of ppid
    └── exitCode                    # attribute of exitCode
```

### 3.3.Description

#### 3.3.1.Interface description
| Interface name | description |
| -------- | -------- |
| const uid :number | returns the digital user ID of the process. |
| const gid :number | returns the numeric group ID of the process. |
| const euid :number | returns the numeric valid user identity of the process. |
| const egid :number | returns the numeric valid group ID of the node.js process. |
| const groups :number[] |  returns an array with supplementary group ID. |
| const pid :number | returns the PID of the process. |
| const ppid :number |  returns the PID of the parent process of the current process. |
| chdir(dir:string) :void | change the current working directory of the node.js process. |
| uptime() :number |  returns the number of seconds the current system has been running. |
| Kill(pid:number, signal:number) :boolean | send the signal to the identified process PID, and true means the sending is successful. |
| abort() :void | cause the node.js process to exit immediately and generate a core file. |
| on(type:string ,listener:EventListener) :void | used to store events triggered by users. |
| exit(code:number):void | cause the node.js process to exit immediately. |
| cwd():string |  returns the current working directory of the node.js process. |
| off(type: string): boolean | clear the events stored by the user. True means the clearing is successful. |
| runCmd(command: string, options?: { timeout : number, killSignal : number \| string, maxBuffer : number }): ChildProcess |through runcmd, you can fork a new process to run a shell and return the childprocess object. The first parameter command refers to the shell to be run, and the second parameter options refers to some running parameters of the child process. These parameters mainly refer to timeout, killsignal and maxbuffer. If timeout is set, the child process will send a signal killsignal after timeout is exceeded. Maxbuffer is used to limit the maximum stdout and stderr sizes that can be received. |
| wait()： Promise\<number> | is used to wait for the child process to run and return the promise object, whose value is the exit code of the child process. |
| getOutput(): Promise\<Uint8Array> |  used to get the standard output of the child process. |
| getErrorOutput(): Promise\<Uint8Array> | used to get the standard error output of the child process. |
| const tid:number | Returns the TID of the process. |
| getStartRealtime() :number | Gets the real time elapsed (in milliseconds) from system startup to process startup. |
| getAvailableCores() :number[] | Gets the CPU kernel available to the current process on the multi-core device. |
| getPastCputime() :number | Gets the CPU time (in milliseconds) from the start of the process to the current time. |
| isIsolatedProcess(): boolean | Check if the process is quarantined. |
| is64Bit(): boolean | Check whether the process is running in a 64 bit environment. |
| isAppUid(v:number): boolean | Checks whether the specified uid belongs to a specific application. |
| getUidForName(v:string): number | Obtain the user group ID to which the user belongs according to the user name |
| getThreadPriority(v:number): number | Gets the thread priority based on the specified TID. |
| getSystemConfig(name:number): number | Gets the configuration of the system according to the specified system configuration name. |
| getEnvironmentVar(name:string): string | Obtain the corresponding value according to the name of the environment variable. |
| close(): void | used to close the running child process. |
| kill(signal: number \| string): void |  used to send signals to child processes. |
| readonly killed: boolean | indicates whether the signal is sent successfully, and true indicates that the signal is sent successfully. |
| readonly exitCode: number | indicates the exit code of the child process. |
| readonly pid: number | represents the child process ID. |
| readonly ppid: number | represents the main process ID. |

#### 3.3.2.Interface instructions

Example of using interface：
1.uid()
```
uid(){
    var res =  Process.uid;
}
```
2.gid()
```
gid(){
    var result = Process.gid;
}
```
3.euid()
```
euid(){
    var and = Process.euid;
}
```
4.egid()
```
egid(){
    var resb = Process.egid;
}
```
5.groups()
```
groups(){
    var answer = Process.groups;
}
```
6.pid()
```
pid(){
    var result = Process.pid;
}
```
7.ppid()
```
ppid(){
    var result = Process.ppid;
}
```
8.chdir()
```
chdir(){
    Process.chdir("123456");
}
```
9.uptime()
```
uptime(){
    var num = Process.uptime();
}
```
10.kill()
```
kill(){
    var ansu = Process.kill(5,23);
}
```
11.abort()
```
abort(){
    Process.abort();
}
```
12.on()
```
on(){
    function add(num){
        var value = num + 5;
        return value;
    }
    Process.on("add",add);
}
```
13.exit()
```
exit(){
    Process.exit(15);
}
```
14.Cwd()
```
Cwd(){
    var result = Process.cwd();
}
```
15.off()

```
off(){
    var result =  Process.off("add");
}
```
16.runCmd()
```
runCmd(){
    var child = process.runCmd('echo abc')
    // killSignal can be a number or a string
    var child = process.runCmd('echo abc;', {killSignal : 'SIGKILL'});
    var child = process.runCmd('sleep 5; echo abc;', {timeout : 1, killSignal : 9, maxBuffer : 2})
}
```
17.wait()
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
18.getOutput()
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
19.getErrorOutput()
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
20.close()
```
close(){
    var child =  process.runCmd('ls; sleep 5s;')
    var result = child.close()
}
```
21.kill()
```
kill(){
    var child =  process.runCmd('ls; sleep 5s;')
    var result = child.kill('SIGHUP');
    child.wait();
    var temp = child.killed;
}
```
22.killed
```
{
    var child = process.runCmd('ls; sleep 5;')
    child.kill(3);
    var killed_ = child.killed;
    child.wait();
}
```
23.exitCode
```
{
    var child = process.runCmd('ls; sleep 5;')
    child.kill(9);
    child.wait();
    var exitCode_ = child.exitCode;
}
```
24.pid
```
pid
{
    var child = process.runCmd('ls; sleep 5;')
    var pid_ = child.pid;
    child.wait();
}
```
25.ppid
```
ppid
{
    var child = process.runCmd('ls; sleep 5;')
    var ppid_ = child.ppid;
    child.wait();
}
```
26.tid
```
tid(){
    var ansu = Process.tid;
}
```
27.isIsolatedProcess()
```
isIsolatedProcess(){
    var ansu = Process.isIsolatedProcess()();
}
```
28.isAppUid()
```
isAppUid(){
    var ansu = Process.isAppUid(10000);
}
```
29.is64Bit()
```
is64Bit(){
    var ansu = Process.is64Bit();
}
```
30.getUidForName()
```
getUidForName(){
    var buf = "root";
    var ansu = Process.getUidForName(buf);
}
```
31.getEnvironmentVar()
```
getEnvironmentVar(){
    var ansu = Process.getEnvironmentVar('USER');
}
```
32.getAvailableCores()
```
getAvailableCores(){
    var ansu = Process.getAvailableCores();
}
```
33.getThreadPriority()
```
getThreadPriority(){
    var result = Process.getTid();
    var ansu = getThreadPriority(result);
}
```
34.getStartRealtime()
```
getStartRealtime(){
    var ansu = Process.getStartRealtime();
}
```
35.getPastCputime()
```
getPastCputime(){
    var ansu = Process.getPastCputime();
}
```
36.getSystemConfig()
```
getSystemConfig(){
    var _SC_ARG_MAX = 0;
    var ansu = Process.getSystemConfig(_SC_ARG_MAX)
}
```

## 4.js_concurrent_module submodule

### 4.1.Introduction

Worker enables JS to have the ability of multithreading, and completes the communication between worker thread and host thread through PostMessage.

### 4.2.Interface description
For interface implementation, see: js_concurrent_module/jsapi/worker

#### 4.2.1.Worker object description

The object object used by the host thread to communicate with the worker thread.

##### 4.2.1.1.Interface

1. 

- name

|constructor(scriptURL:string, options? WorkerOptions) | worker constructor to Creates a worker instance |
|---|---|

- example

```
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
```

2. 

- name

| postMessage(message:Object, options?:PostMessageOptions)  | Sends a message to the worker thread |
|---|---|
| postMessage(message:Object, transfer:ArrayBuffer[])  | Sends a message to the worker thread |

- example

```
// example 1
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
worker.postMessage("hello world");
 
// example 2
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
var buffer = new ArrayBuffer(8);
worker.postMessage(buffer, [buffer]);
```

3. 

- name

| on(type:string, listener:EventListener)  | Adds an event listener to the worker  |
|---|---|

- example

```
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
worker.on("alert", (e)=>{
     console.log("worker on...");
})
```

4. 

- name

| once(type:string, listener:EventListener)  | Adds an event listener to the worker and removes the event listener automically after it is invoked once  |
|---|---|

- example

```
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
worker.once("alert", (e)=>{
    console.log("worker on...");
})
```

5. 

- name

| off(type:string, listener?:EventListener)  | Removes an event listener to the worker  |
|---|---|

- example

```
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
worker.off("alert");
```

6. 

- name

| terminate()  | Terminates the worker thread to stop the worker from receiving messages  |
|---|---|

- example

```
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
worker.terminate();
```

7. 

- name

| removeEventListener(type:string, listener?:EventListener)  | Removes an event defined for the worker  |
|---|---|

- example

```
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
worker.removeEventListener("alert");
```

8. 

- name

| dispatchEvent(event: Event)  | Dispatches the event defined for the worker  |
|---|---|

- example

```
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
worker.dispatchEvent({type:"alert"});
```

9. 

- name

| removeAllListener()  | Removes all event listeners for the worker  |
|---|---|

- example

```
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
worker.removeAllListener();
```

##### 4.2.1.2.Attribute

1. 

- name

| onexit?:(code:number)=>void  | The onexit attribute of the worker specifies the event handler to be called when the worker exits. The handler is executed in the host thread  |
|---|---|

- example

```
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
worker.onexit = function(e) {
    console.log("onexit...");
}
```

2. 

- name

| onerror?:(ev:ErrorEvent)=>void  |  The onerror attribute of the worker specifies the event handler to be called when an exception occurs during worker execution. The event handler is executed in the host thread |
|---|---|

- example

```
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
worker.onerror = function(e) {
    console.log("onerror...");
}
```

3. 

- name

| onmessage?:(ev:MessageEvent)=>void  | The onmessage attribute of the worker specifies the event handler to be called then the host thread receives a  message created by itself and sent by the worker through the parentPort.postMessage. The event handler is executed in the host thread  |
|---|---|

- example

```
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
worker.onmessage = function(e) {
    console.log("onmessage...");
}
```

4. 

- name

| onmessageerror?:(event:MessageEvent)=>void  | The onmessage attribute of the worker specifies the event handler when the worker receives a message that   cannot be serialized. The event handler is executed in the host thread |
|---|---|

- example

```
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
worker.onmessageerror = function(e) {
    console.log("onmessageerror...");
}
```

#### 4.2.2.parentPort object description

Object of the worker thread used to communicate with the host thread

##### 4.2.2.1.Interface

1. 

- name

| postMessage(message:Object, options?:PostMessageOptions)  | Send a message to host thread  |
|---|---|
| postMessage(message:Object, transfer:ArrayBuffer[])  | Send a message to host thread |

- example

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

- name

| close()  | Close the worker thread to stop the worker from receiving messages  |
|---|---|

- example

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

##### 4.2.2.2.Attribute

1. 

- name

| onmessage?:(event:MessageEvent)=>void  | The onmessage attribute of parentPort specifies the event handler to be called then the worker thread receives a message sent by the host thread through worker postMessage. The event handler is executed in the worker thread  |
|---|---|

- example

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

- name

| onerror?:(ev: ErrorEvent)=>void  | The onerror attribute of parentPort specifies the event handler to be called when an exception occurs during worker execution. The event handler is executed in the worker thread  |
|---|---|

- example

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

- name

| onmessageerror?:(event: MessageEvent)=>void  | The onmessage attribute of parentPort specifies the event handler to be called then the worker receives a message that cannot be deserialized. The event handler is executed in the worker thread.  |
|---|---|

- example

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

### Repositories Involved

- ace_ace_engine(foundation/arkui/ace_engine-readme.md)
- ace_napi(foundation/arkui/napi-readme.md)

## Related warehouse
[ets_utils Subsystem](commonlibrary/ets_utils/README.en.md)

## License

URL is available under [Mozilla license](https://www.mozilla.org/en-US/MPL/), and the documentation is detailed in [documentation](https://gitee.com/openharmony/js_api_module/blob/master/mozilla_docs.txt). See [LICENSE](https://gitee.com/openharmony-sig/commonlibrary_ets_utils/blob/master/LICENSE) for the full license text.
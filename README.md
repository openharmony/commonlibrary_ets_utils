# js_api_module Subsystem/Component

-   [Introduction](#Introduction)
-   [Contents](#Contents)
-   [Illustrate](#Illustrate)
    -   [Interface Description](#Interface Description)
    -   [Instructions for use](#Instructions for use)

-   [Related warehouse](#Related warehouse)

## Introduction

The interface of URL is used to parse, construct, normalize, and encode URLs. The constructor of URL creates a new URL object. In order to make changes to the resolved components of the URL or to the URL. The URLSearchParams interface defines some practical methods to process URL query strings.

URI Represents a Uniform Resource Identifier (URI) reference.

XML representation refers to extensible markup language.

The XmlSerializer interface is used to generate an xml file. The constructor of XmlSerializer creates a new XmlSerializer object, and calls the method of the XmlSerializer object to generate an xml file. The XmlPullParser interface is used to parse the existing xml file. The constructor of XmlPullParser creates a new XmlPullParser object, and calls the method of the XmlPullParser object to parse the xml file.

## Contents

```
base/compileruntime/js_api_module/
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

## Illustrate

### Interface Description


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
### Instructions for use

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
            '    <todo>Work</todo>' +
            '    <todo>Play</todo>' +
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
            '    <todo>Work</todo>' +
            '    <todo>Play</todo>' +
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
## Related warehouse
[js_api_module Subsystem](base/compileruntime/js_api_module-readme.md)

## License

URL is available under [Mozilla license](https://www.mozilla.org/en-US/MPL/), and the documentation is detailed in [documentation](https://gitee.com/openharmony/js_api_module/blob/master/mozilla_docs.txt). See [LICENSE](https://gitee.com/openharmony/js_api_module/blob/master/LICENSE) for the full license text.
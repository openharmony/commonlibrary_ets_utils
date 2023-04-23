# ets_utils组件
ets_utils组件共提供四个子模块，分别是：js_api_module、js_util_module、js_sys_module和js_worker_module，以下内容将详细介绍各个子模块的简介、目录、说明和涉及仓等相关内容。

# 子模块
## 1. js_api_module子模块
### 1.1. 简介

URL接口用于解析，构造，规范化和编码 URLs。 URL的构造函数创建新的URL对象。 以便对URL的已解析组成部分或对URL进行更改。URLSearchParams 接口定义了一些实用的方法来处理 URL 的查询字符串。

URI表示统一资源标识符引用。

xml表示指可扩展标记语言。

XmlSerializer接口用于生成一个xml文件。 XmlSerializer的构造函数创建新的XmlSerializer对象，调用XmlSerializer对象的方法生成一个xml文件。XmlPullParser 接口用于解析已有的xml文件，XmlPullParser的构造函数创建新的XmlPullParser对象，调用XmlPullParser对象的方法解析xml。

### 1.2. 目录

```
commonlibrary/ets_utils/js_api_module/
├── Class:URL                                                                     # URL类
│   ├── new URL(input[, base])                                                    # 创建URL对象
│   ├── hash                                                                      # hash属性
│   ├── host                                                                      # host属性
│   ├── hostname                                                                  # hostname属性
│   ├── href                                                                      # href属性
│   ├── origin                                                                    # origin属性
│   ├── password                                                                  # password属性
│   ├── pathname                                                                  # pathname属性
│   ├── port                                                                      # port属性
│   ├── protocol                                                                  # protocol属性
│   ├── search                                                                    # search属性
│   ├── searchParams                                                              # searchParams属性
│   ├── username                                                                  # username属性
│   ├── toString()                                                                # toString方法
│   └── toJSON()                                                                  # toJSON方法
├── Class: URLSearchParams                                                        # URLSearchParams类
│   ├── new URLSearchParams()                                                     # 创建URLSearchParams对象
│   ├── new URLSearchParams(string)                                               # 创建URLSearchParams对象
│   ├── new URLSearchParams(obj)                                                  # 创建URLSearchParams对象
│   ├── new URLSearchParams(iterable)                                             # 创建URLSearchParams对象
│   ├── append(name, value)                                                       # append方法
│   ├── delete(name)                                                              # delete方法
│   ├── entries()                                                                 # entries方法
│   ├── forEach(fn[, thisArg])                                                    # forEach方法
│   ├── get(name)                                                                 # get方法
│   ├── getAll(name)                                                              # getAll方法
│   ├── has(name)                                                                 # has方法
│   ├── keys()                                                                    # keys方法
│   ├── set(name, value)                                                          # set方法
│   ├── sort()                                                                    # sort方法
│   ├── toString()                                                                # toString方法
│   ├── values()                                                                  # values方法
│   └── urlSearchParams[Symbol.iterator]()                                        # 创建URLSearchParams对象
├── Class:URI                                                                     # URI类
│   ├── URI(str: string)                                                          # 创建URI对象
│   ├── scheme                                                                    # scheme属性
│   ├── authority                                                                 # authority属性
│   ├── ssp                                                                       # ssp属性
│   ├── userinfo                                                                  # userinfo属性
│   ├── host                                                                      # host属性
│   ├── port                                                                      # port属性
│   ├── query                                                                     # query属性
│   ├── fragment                                                                  # fragment属性
│   ├── path                                                                      # path属性
│   ├── equals(ob: Object)                                                        # equals方法
│   ├── normalize()                                                               # normalize方法
│   ├── checkIsAbsolute()                                                         # checkIsAbsolute方法
│   ├── normalize()                                                               # normalize方法
│   └── toString()                                                                # toString方法
├── Class:ConvertXml                                                              # ConvertXml类
│   ├── ConvertXml()                                                              # 创建ConvertXml类对象
│   └── convert(xml: string, options: Object)                                     # convert方法
├── Class:XmlSerializer                                                           # XmlSerializer类
│   ├── new XmlSerializer(buffer: ArrayBuffer | DataView, encoding?: string)      # 创建XmlSerializer类对象
│   ├── setAttributes(name: string, value: string)                                # 设置Attributes方法
│   ├── addEmptyElement(name: string)                                             # 添加一个空元素方法
│   ├── setDeclaration()                                                          # 设置Declaration方法
│   ├── startElement(name: string)                                                # 设置开始元素方法
│   ├── endElement()                                                              # 设置结束元素方法
│   ├── setNamespace(prefix: string, namespace: string)                           # 设置命名空间方法
│   ├── setCommnet(text: string)                                                  # 设置Commnet方法
│   ├── setCData(text: string)                                                    # 设置CData方法
│   ├── setText(text: string)                                                     # 设置Text方法
│   └── setDocType(text: string)                                                  # 设置DocType方法
└── Class: XmlPullParser                                                          # XmlPullParser类
    ├── new (buffer: ArrayBuffer | DataView, encoding?: string)                   # 创建XmlPullParser对象
    └── parse(option: ParseOptions)                                               # parse方法
```

### 1.3. 说明

#### 1.3.1. 接口说明


| 接口名 | 说明 |
| -------- | -------- |
| URL(url: string,base?:string \| URL) | 创建并返回一个URL对象，该URL对象引用使用绝对URL字符串，相对URL字符串和基本URL字符串指定的URL。 |
| tostring():string | 该字符串化方法返回一个包含完整 URL 的 USVString。它的作用等同于只读的 URL.href。 |
| toJSON():string | 该方法返回一个USVString，其中包含一个序列化的URL版本。 |
| new URLSearchParams() | URLSearchParams() 构造器无入参，该方法创建并返回一个新的URLSearchParams 对象。 开头的'?' 字符会被忽略。 |
| new URLSearchParams(string) | URLSearchParams(string) 构造器的入参为string数据类型，该方法创建并返回一个新的URLSearchParams 对象。 开头的'?' 字符会被忽略。 |
| new URLSearchParams(obj) | URLSearchParams(obj) 构造器的入参为obj数据类型，该方法创建并返回一个新的URLSearchParams 对象。 开头的'?' 字符会被忽略。 |
| new URLSearchParams(iterable) | URLSearchParams(iterable) 构造器的入参为iterable数据类型，该方法创建并返回一个新的URLSearchParams 对象。 开头的'?' 字符会被忽略。 |
| has(name: string): boolean | 检索searchParams对象中是否含有name。有则返回true，否则返回false。 |
| set(name: string, value string): void |  检索searchParams对象中是否含有key为name的键值对。没有的话则添加该键值对，有的话则修改对象中第一个key所对应的value，并删除键为name的其余键值对。 |
| sort(): void | 根据键的Unicode代码点，对包含在此对象中的所有键/值对进行排序，并返回undefined。 |
| toString(): string | 根据searchParams对象,返回适用在URL中的查询字符串。 |
| keys(): iterableIterator\<string> | 返回一个iterator，遍历器允许遍历对象中包含的所有key值。 |
| values(): iterableIterator\<string> | 返回一个iterator，遍历器允许遍历对象中包含的所有value值。 |
| append(name: string, value: string): void | 在searchParams对象中插入name, value键值对。 |
| delete(name: string): void | 遍历searchParams对象，查找所有的name,删除对应的键值对。 |
| get(name: string): string | 检索searchParams对象中第一个name,返回name键对应的值。 |
| getAll(name: string): string[] | 检索searchParams对象中所有name,返回name键对应的所有值。 |
| entries(): iterableIterator<[string, string]> | 返回一个iterator，允许遍历searchParams对象中包含的所有键/值对。 |
| forEach(): void | 通过回调函数来遍历URLSearchParams实例对象上的键值对。 |
| urlSearchParams\[Symbol.iterator]() | 返回查询字符串中每个名称-值对的ES6迭代器。迭代器的每个项都是一个JavaScript数组。 |
| URI​(str: string) | 通过解析给定入参（String str）来构造URI。此构造函数严格按照RFC 2396附录A中的语法规定解析给定字符串。 |
| scheme​ | 返回此 URI 的scheme部分，如果scheme未定义，则返回 null |
| authority​ | 返回此 URI 的解码authority部分，如果authority未定义，则返回 null。 |
| ssp​ |  返回此 URI 的解码scheme-specific部分。 |
| userinfo​ | 返回此 URI 的解码userinfo部分。包含passworld和username。 |
| host​ | 返回此 URI 的host部分，如果host未定义，则返回 null。 |
| port​ | 返回此 URI 的port部分，如果port未定义，则返回 -1。URI 的port组件（如果已定义）是一个非负整数。 |
| query​ | 返回此 URI 的query部分，如果query未定义，则返回 null。 |
| fragment​ | 返回此 URI 的解码fragment组件，如果fragment未定义，则返回 null。|
| path​ | 返回此 URI 的解码path组件，如果path未定义，则返回 null。 |
| equals(ob: Object) | 测试此 URI 是否与另一个对象相等。如果给定的对象不是 URI，则此方法立即返回 false。 |
| normalize​() | 规范化这个 URI 的路径。如果这个 URI 的path不规范，将规范后构造一个新 URI对象返回。 |
| checkIsAbsolute​() | 判断这个 URI 是否是绝对的。当且仅当它具有scheme部分时，URI 是绝对的，返回值为true，否则返回值为false。 |
| ConvertXml() | 用于构造ConvertXml类对象的构造函数。此构造函数无需传入参数。 |
| convert(xml: string, options: Object)  | 返回按选项要求转化xml字符串的JavaScrip对象。 |
| XmlSerializer(buffer: ArrayBuffer \| DataView, encoding?: string) | 创建并返回一个XmlSerializer对象，该XmlSerializer对象传参两个第一参数是ArrayBuffer或DataView一段内存，第二个参数为文件格式（默认为UTF-8）。 |
| setAttributes(name: string, value: string): void | 给xml文件中写入属性Attributes属性。 |
| addEmptyElement(name: string): void | 写入一个空元素。 |
| setDeclaration(): void | 设置Declaration使用编码写入xml声明。例如：<？xml version=“1.0”encoding=“utf-8”> |
| startElement(name: string): void | 写入具有给定名称的elemnet开始标记。|
| endElement(): void | 写入元素的结束标记。 |
| setNamespace(prefix: string, namespace: string): void | 写入当前元素标记的命名空间。 |
| setCommnet(text: string): void | 写入comment属性。 |
| setCData(text: string): void | 写入CData属性。 |
| setText(text: string): void | 写入Text属性。 |
| setDocType(text: string): void | 写入DocType属性。 |
| XmlPullParser(buffer: ArrayBuffer \| DataView, encoding?: string) | 创建并返回一个XmlPullParser对象，该XmlPullParser对象传参两个第一参数是ArrayBuffer或DataView一段内存，第二个参数为文件格式（默认为UTF-8）。 |
| parse(option: ParseOptions): void | 该接口用于解析xml，ParseOptions参数为一个接口包含五个可选参{supportDoctype?: boolea ignoreNameSpace?: boolean tagValueCallbackFunction?: (name: string, value: string) => boolean attributeValueCallbackFunction?: (name: string, value: string) => boolean） tokenValueCallbackFunction?: (eventType: EventType, value: ParseInfo) => boolean }。其中tokenValueCallbackFunction回调函数的入参1是事件类型，入参2是包含getColumnNumber、getDepth等get属性的info接口，用户可通过info.getDepth()等方法来获取当前解析过程中的depth等信息。 |
#### 1.3.2. 使用说明

各接口使用方法如下：


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


21、URI​(str: string)
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
31、equals(ob: Object)
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
33、checkIsAbsolute​()
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
var options = {supportDoctype:true, ignoreNameSpace:true, tokenValueCallbackFunction:func}
that.parse(options);
```

## 2. js_util_module子模块
### 2.1. 简介

UTIL接口用于字符编码TextEncoder、解码TextDecoder、帮助函数HelpFunction、基于Base64的字节编码encode和解码decode、有理数RationalNumber。TextEncoder表示一个文本编码器，接受字符串作为输入，以UTF-8格式进行编码，输出UTF-8字节流。TextDecoder接口表示一个文本解码器，解码器将字节流作为输入，输出stirng字符串。HelpFunction主要是对函数做callback化、promise化以及对错误码进行编写输出，及类字符串的格式化输出。encode接口使用Base64编码方案将指定u8数组中的所有字节编码到新分配的u8数组中或者使用Base64编码方案将指定的字节数组编码为String。decode接口使用Base64编码方案将Base64编码的字符串或输入u8数组解码为新分配的u8数组。RationalNumber有理数主要是对有理数进行比较，获取分子分母等方法。LruBuffer该算法在缓存空间不够的时候，将近期最少使用的数据替换为新数据。该算法源自这样一种访问资源的需求：近期访问的数据，可能在不久的将来会再次访问。于是最少访问的数据就是价值最小的，是最应该踢出缓存空间的数据。Scope接口用于描述一个字段的有效范围。 Scope实例的构造函数用于创建具有指定下限和上限的对象，并要求这些对象必须具有可比性。
### 2.2. 目录

```
commomlibrary/ets_utils/js_util_module/
├── Class:TextEncoder                   # TextEncoder类
│   ├──  new TextEncoder()              # 创建TextEncoder对象
│   ├──  encode()                       # encode方法
│   ├──  encoding                       # encoding属性
│   └──  encodeInto()                   # encodeInto方法
├── Class:TextDecoder                   # TextDecoder类
│   ├──  new TextDecoder()              # 创建TextDecoder对象
│   ├──  decode()                       # decode方法
|   ├──  decodeWithStream()             # decodeWithStream方法
│   ├──  encoding                       # encoding属性
│   ├──  fatal                          # fatal属性
│   └──  ignoreBOM                      # ignoreBOM属性
├── printf()                            # printf方法
├── getErrorString()                    # getErrorString方法
├── callbackWrapper()                   # callbackWrapper方法
├── promiseWrapper()                    # promiseWrapper方法
├── Class:Base64                        # Base64类
│   ├──  new Base64()                   # 创建Base64对象
│   ├──  encodeSync()                   # encodeSync方法
│   ├──  encodeToStringSync()           # encodeToStringSync方法
│   ├──  decodeSync()                   # decodeSync方法
│   ├──  encode()                       # encode方法
│   ├──  encodeToString()               # encodeToString方法
│   └──  decode()                       # decode方法
├── Class:RationalNumber                # RationalNumber类
│   ├──  new RationalNumber()           # 创建RationalNumber对象
│   ├──  createRationalFromString()     # createRationalFromString方法
│   ├──  compareTo()                    # compareTo方法
│   ├──  equals()                       # equals方法
│   ├──  valueOf()                      # valueOf方法
│   ├──  getCommonDivisor()             # getCommonDivisor方法
│   ├──  getDenominator()               # getDenominator方法
│   ├──  getNumerator()                 # getNumerator方法
│   ├──  isFinite()                     # isFinite方法
│   ├──  isNaN()                        # isNaN方法
│   ├──  isZero()                       # isZero方法
│   └──  toString()                     # toString方法
├── Class:LruBuffer                     # LruBuffer类
│   ├──  new LruBuffer()                # 创建LruBuffer对象
│   ├──  updateCapacity()               # updateCapacity方法
│   ├──  toString()                     # toString方法
│   ├──  values()                       # values方法
│   ├──  length                         # length属性
│   ├──  getCapacity()                  # getCapacity方法
│   ├──  clear()                        # clear方法
│   ├──  getCreateCount()               # getCreateCount方法
│   ├──  getMissCount()                 # getMissCount方法
│   ├──  getRemovalCount()              # getRemovalCount方法
│   ├──  getMatchCount()                # getMatchCount方法
│   ├──  getPutCount()                  # getPutCount方法
│   ├──  isEmpty()                      # isEmpty方法
│   ├──  get()                          # get方法
│   ├──  put()                          # put方法
│   ├──  keys()                         # keys方法
│   ├──  remove()                       # remove方法
│   ├──  afterRemoval()                 # afterRemoval方法
│   ├──  contains()                     # contains方法
│   ├──  createDefault()                # createDefault方法
│   ├──  entries()                      # entries方法
│   └──  [Symbol.iterator]()            # Symboliterator方法
|—— Class:Scope                         # Scope类
|   ├── constructor()                   # 创建Scope对象
|   ├── toString()                      # toString方法
|   ├── intersect()                     # intersect方法
|   ├── intersect()                     # intersect方法
|   ├── getUpper()                      # getUpper方法
|   ├── getLower()                      # getLower方法
|   ├── expand()                        # expand方法
|   ├── expand()                        # expand方法
|   ├── expand()                        # expand法
|   ├── contains()                      # contains方法
|   ├── contains()                      # contains方法
|   └── clamp()                         # clamp方法
|—— Class:Types                         # Types类
|   ├── isAnyArrayBuffer()              # isAnyArrayBuffer方法
|   ├── isArrayBufferView()             # isArrayBufferView方法
|   ├── isArgumentsObject()             # isArgumentsObject方法
|   ├── isArrayBuffer()                 # isArrayBuffer方法
|   ├── isAsyncFunction()               # isAsyncFunction方法
|   ├── isBigInt64Array()               # isBigInt64Array方法
|   ├── isBigUint64Array()              # isBigUint64Array方法
|   ├── isBooleanObject()               # isBooleanObject方法
|   ├── isBoxedPrimitive()              # isBoxedPrimitive方法
|   ├── isDataView()                    # isDataView方法
|   ├── isDate()                        # isDate方法
|   ├── isExternal()                    # isExternal方法
|   ├── isFloat32Array()                # isFloat32Array方法
|   ├── isFloat64Array()                # isFloat64Array方法
|   ├── isGeneratorFunction()           # isGeneratorFunction方法
|   ├── isGeneratorObject()             # isGeneratorObject方法
|   ├── isInt8Array()                   # isInt8Array方法
|   ├── isInt16Array()                  # isInt16Array方法
|   ├── isInt32Array()                  # isInt32Array方法
|   ├── isMap()                         # isMap方法
|   ├── isMapIterator()                 # isMapIterator方法
|   ├── isModuleNamespaceObject()       # isModuleNamespaceObject方法
|   ├── isNativeError()                 # isNativeError方法
|   ├── isNumberObject()                # isNumberObject方法
|   ├── isPromise()                     # isPromise方法
|   ├── isProxy()                       # isProxy方法
|   ├── isRegExp()                      # isRegExp方法
|   ├── isSet()                         # isSet方法
|   ├── isSetIterator()                 # isSetIterator方法
|   ├── isSharedArrayBuffer()           # isSharedArrayBuffer方法
|   ├── isStringObject()                # isStringObject方法
|   ├── isSymbolObject()                # isSymbolObject方法
|   ├── isTypedArray()                  # isTypedArray方法
|   ├── isUint8Array()                  # isUint8Array方法
|   ├── isUint8ClampedArray()           # isUint8ClampedArray方法
|   ├── isUint16Array()                 # isUint16Array方法
|   ├── isUint32Array()                 # isUint32Array方法
|   ├── isWeakMap()                     # isWeakMap方法
|   └── isWeakSet()                     # isWeakSet方法
|—— Class:ArrayList 
|   ├── new ArrayList()                 # 创建ArrayList对象
|   ├── length                          # length属性
|   ├── add()                           # add方法
|   ├── insert()                        # insert方法
|   ├── has()                           # has方法
|   ├── getIndexOf()                    # getIndexOf方法
|   ├── removeByIndex()                 # removeByIndex方法
|   ├── remove()                        # remove方法
|   ├── getLastIndexOf()                # getLastIndexOf方法
|   ├── removeByRange()                 # removeByRange方法
|   ├── replaceAllElements()            # replaceAllElements方法
|   ├── forEach()                       # forEach方法
|   ├── sort()                          # sort方法
|   ├── subArrayList()                  # subArrayList方法
|   ├── clear()                         # clear方法
|   ├── clone()                         # clone方法
|   ├── getCapacity()                   # getCapacity方法
|   ├── convertToArray()                # convertToArray方法
|   ├── isEmpty()                       # isEmpty方法
|   ├── increaseCapacityTo()            # increaseCapacityTo方法
|   ├── trimToCurrentLength()           # trimToCurrentLength方法
|   └── isEmpty()                       # isEmpty方法
|—— Class:Deque
|   ├── new Deque()                     # 创建Deque对象
|   ├── length                          # length属性
|   ├── add()                           # add方法
|   ├── insertFront()                   # insertFront方法
|   ├── insertEnd()                     # insertEnd方法
|   ├── has()                           # has方法
|   ├── getFirst()                      # getFirst方法
|   ├── getLast()                       # getLast方法
|   ├── popFirst()                      # popFirst方法
|   ├── popLast()                       # popLast方法
|   └── forEach()                       # forEach方法
|—— Class:HashMap
|   ├── new HashMap()                   # 创建HashMap对象
|   ├── length                          # length属性
|   ├── isEmpty()                       # isEmpty方法
|   ├── hasKey()                        # hasKey方法
|   ├── hasValue()                      # hasValue方法
|   ├── setAll()                        # setAll方法
|   ├── set()                           # set方法
|   ├── remove()                        # remove方法
|   ├── clear()                         # clear方法
|   ├── keys()                          # keys方法
|   ├── values()                        # values方法
|   ├── replace()                       # replace方法
|   ├── forEach()                       # forEach方法
|   └── entries()                       # entries方法
|—— Class:HashSet
|   ├── new HashSet()                   # 创建HashSet对象
|   ├── length                          # length属性
|   ├── isEmpty()                       # isEmpty方法
|   ├── has()                           # has方法
|   ├── add()                           # add方法
|   ├── remove()                        # remove方法
|   ├── clear()                         # clear方法
|   ├── forEach()                       # forEach方法
|   ├── values()                        # values方法
|   └── entries()                       # entries方法
|—— Class:LightWeightMap
|   ├── new LightWeightMap()            # 创建LightWeightMap对象
|   ├── length                          # length属性
|   ├── hasAll()                        # hasAll方法
|   ├── hasKey()                        # hasKey方法
|   ├── hasValue()                      # hasValue方法
|   ├── increaseCapacityTo()            # increaseCapacityTo方法
|   ├── entries()                       # entries方法
|   ├── get()                           # get方法
|   ├── getIndexOfKey()                 # getIndexOfKey方法
|   ├── getIndexOfValue()               # getIndexOfValue方法
|   ├── isEmpty()                       # isEmpty方法
|   ├── getKeyAt()                      # getKeyAt方法
|   ├── keys()                          # keys方法
|   ├── setAll()                        # setAll方法
|   ├── set()                           # set方法
|   ├── remove()                        # remove方法
|   ├── removeAt()                      # removeAt方法
|   ├── clear()                         # clear方法
|   ├── setValueAt()                    # setValueAt方法
|   ├── forEach()                       # forEach方法
|   ├── toString()                      # toString方法
|   ├── getValueAt()                    # getValueAt方法
|   ├── values()                        # values方法
|   └── [Symbol.iterator]()             # [Symbol.iterator]方法
|—— Class:LightWeightSet
|   ├── new LightWeightSet()            # 创建LightWeightSet对象
|   ├── length                          # length属性
|   ├── add()                           # add方法
|   ├── addAll()                        # addAll方法
|   ├── hasAll()                        # hasAll方法
|   ├── has()                           # has方法
|   ├── equal()                         # equal方法
|   ├── increaseCapacityTo()            # increaseCapacityTo方法
|   ├── getIndexOf()                    # getIndexOf方法
|   ├── remove()                        # remove方法
|   ├── removeAt()                      # removeAt方法
|   ├── clear()                         # clear方法
|   ├── forEach()                       # forEach方法
|   ├── [Symbol.iterator]()             # [Symbol.iterator]方法
|   ├── toString()                      # toString方法
|   ├── toArray()                       # toArray方法
|   ├── getValueAt()                    # getValueAt方法
|   ├── values()                        # values方法
|   ├── entries()                       # entries方法
|   └── isEmpty()                       # isEmpty方法
|—— Class:LinkedList
|   ├── new LinkedList()                # 创建LinkedList对象
|   ├── length                          # length属性
|   ├── add()                           # add方法
|   ├── insert()                        # insert方法
|   ├── get()                           # get方法
|   ├── addFirst()                      # addFirst方法
|   ├── removeFirst()                   # removeFirst方法
|   ├── removeLast()                    # removeLast方法
|   ├── has()                           # has方法
|   ├── getIndexOf()                    # getIndexOf方法
|   ├── removeByIndex()                 # removeByIndex方法
|   ├── remove()                        # remove方法
|   ├── removeFirstFound()              # removeFirstFound方法
|   ├── removeLastFound()               # removeLastFound方法
|   ├── getLastIndexOf()                # getLastIndexOf方法
|   ├── getFirst()                      # getFirst方法
|   ├── getLast()                       # getLast方法
|   ├── set()                           # set方法
|   ├── forEach()                       # forEach方法
|   ├── clear()                         # clear方法
|   ├── clone()                         # clone方法
|   ├── convertToArray()                # convertToArray方法
|   └── [Symbol.iterator]()             # [Symbol.iterator]方法
|—— Class:List
|   ├── new List()                      # 创建List对象
|   ├── length                          # length属性
|   ├── add()                           # add方法
|   ├── insert()                        # insert方法
|   ├── get()                           # get方法
|   ├── has()                           # has方法
|   ├── getIndexOf()                    # getIndexOf方法
|   ├── removeByIndex()                 # removeByIndex方法
|   ├── remove()                        # remove方法
|   ├── getLastIndexOf()                # getLastIndexOf方法
|   ├── getFirst()                      # getFirst方法
|   ├── getLast()                       # getLast方法
|   ├── set()                           # set方法
|   ├── equal()                         # equal方法
|   ├── forEach()                       # forEach方法
|   ├── sort()                          # sort方法
|   ├── clear()                         # clear方法
|   ├── getSubList()                    # getSubList方法
|   ├── replaceAllElements()            # replaceAllElements方法
|   ├── convertToArray()                # convertToArray方法
|   ├── isEmpty()                       # isEmpty方法
|   └── [Symbol.iterator]()             # [Symbol.iterator]方法
|—— Class:PlainArray
|   ├── new PlainArray()                # 创建PlainArray对象
|   ├── length                          # length属性
|   ├── add()                           # add方法
|   ├── clear()                         # clear方法
|   ├── clone()                         # clone方法
|   ├── has()                           # has方法
|   ├── get()                           # get方法
|   ├── getIndexOfKey()                 # getIndexOfKey方法
|   ├── getIndexOfValue()               # getIndexOfValue方法
|   ├── isEmpty()                       # isEmpty方法
|   ├── getKeyAt()                      # getKeyAt方法
|   ├── remove()                        # remove方法
|   ├── removeAt()                      # removeAt方法
|   ├── removeRangeFrom()               # removeRangeFrom方法
|   ├── setValueAt()                    # setValueAt方法
|   ├── toString()                      # toString方法
|   ├── getValueAt()                    # getValueAt方法
|   ├── forEach()                       # forEach方法
|   └── [Symbol.iterator]()             # [Symbol.iterator]方法
|—— Class:Queue
|   ├── new Queue()                     # 创建Queue对象
|   ├── length                          # length属性
|   ├── add()                           # add方法
|   ├── getFirst()                      # getFirst方法
|   ├── pop()                           # pop方法
|   ├── forEach()                       # forEach方法
|   └── [Symbol.iterator]()             # [Symbol.iterator]方法
|—— Class:Stack
|   ├── new Stack()                     # 创建Stack对象
|   ├── length                          # length属性
|   ├── isEmpty()                       # isEmpty方法
|   ├── peek()                          # peek方法
|   ├── pop()                           # pop方法
|   ├── push()                          # push方法
|   ├── locate()                        # locate方法
|   ├── forEach()                       # forEach方法
|   └── [Symbol.iterator]()             # [Symbol.iterator]方法
|—— Class:TreeMap
|   ├── new TreeMap()                   # 创建TreeMap对象
|   ├── length                          # length属性
|   ├── isEmpty()                       # isEmpty方法
|   ├── hasKey()                        # hasKey方法
|   ├── hasValue()                      # hasValue方法
|   ├── get()                           # get方法
|   ├── getFirstKey()                   # getFirstKey方法
|   ├── getLastKey()                    # getLastKey方法
|   ├── setAll()                        # setAll方法
|   ├── set()                           # set方法
|   ├── remove()                        # remove方法
|   ├── clear()                         # clear方法
|   ├── getLowerKey()                   # getLowerKey方法
|   ├── getHigherKey()                  # getHigherKey方法
|   ├── keys()                          # keys方法
|   ├── values()                        # values方法
|   ├── replace()                       # replace方法
|   ├── forEach()                       # forEach方法
|   ├── entries()                       # entries方法
|   └── [Symbol.iterator]()             # [Symbol.iterator]方法
└── Class:TreeSet
    ├── new TreeSet()                   # 创建TreeSet对象
    ├── length                          # length属性
    ├── isEmpty()                       # isEmpty方法
    ├── has()                           # has方法
    ├── add()                           # add方法
    ├── remove()                        # remove方法
    ├── clear()                         # clear方法
    ├── getFirstValue()                 # getFirstValue方法
    ├── getLastValue()                  # getLastValue方法
    ├── getLowerValue()                 # getLowerValue方法
    ├── getHigherValue()                # getHigherValue方法
    ├── popFirst()                      # popFirst方法
    ├── popLast()                       # popLast方法
    ├── forEach()                       # forEach方法
    ├── values()                        # values方法
    ├── entries()                       # entries方法
    └── [Symbol.iterator]()             # [Symbol.iterator]方法
```

### 2.3. 说明

#### 2.3.1. 接口说明


| 接口名 | 说明 |
| -------- | -------- |
| constructor(encoding? : string) | 构造函数，参数encoding表示编码的格式。默认utf-8, 支持gb18030, gbk, gb2312. |
| readonly encoding : string | 在TextEncoder类中，获取编码的格式，只支持UTF-8。 |
| encode(input : string) : Uint8Array | 输入stirng字符串，根据encodeing编码并输出uint8字节流。 |
| encodeInto(input : string, dest : Uint8Array) : {read : number, written : number} | 输入stirng字符串，dest表示编码后存放位置，返回一个对象，read表示已经编码的字符的个数，written表示已编码字符所占字节的大小。 |
| constructor(encoding? : string, options? : {fatal? : boolean, ignoreBOM? : boolean}) | 构造函数，第一个参数encoding表示解码的格式。第二个参数表示一些属性。属性中fatal表示是否抛出异常，ignoreBOM表示是否忽略bom标志。 |
| readonly encoding : string | 在TextDecoder类中，获取设置的解码格式。 |
| readonly fatal : boolean | 获取抛出异常的设置。 |
| readonly ignoreBOM : boolean | 获取是否忽略bom标志的设置。 |
| decode(input : Uint8Array, options?: { stream?: false }) : string | 输入要解码的数据，解出对应的string字符串。第一个参数input表示要解码的数据，第二个参数options表示一个bool标志，表示将跟随附加数据，默认为false。 |
| decodeWithStream(input : Uint8Array, options?: { stream?: false }) : string | 输入要解码的数据，解出对应的string字符串。第一个参数input表示要解码的数据，第二个参数options表示一个bool标志，表示将跟随附加数据，默认为false。 |
| encodeSync(src: Uint8Array): Uint8Array; | 使用Base64编码方案将指定u8数组中的所有字节编码到新分配的u8数组中。 |
| encodeToStringSync(src: Uint8Array): string; | 使用Base64编码方案将指定的字节数组编码为String。 |
| decodeSync(src: Uint8Array \| string): Uint8Array; | 使用Base64编码方案将Base64编码的字符串或输入u8数组解码为新分配的u8数组。 |
| encode(src: Uint8Array): Promise\<Uint8Array\>; | 使用Base64编码方案将指定u8数组中的所有字节异步编码到新分配的u8数组中。 |
| encodeToString(src: Uint8Array): Promise\<string\>; | 使用Base64编码方案将指定的字节数组异步编码为String。 |
| decode(src: Uint8Array \| string): Promise\<Uint8Array\>; | 使用Base64编码方案将Base64编码的字符串或输入u8数组异步解码为新分配的u8数组。 |
| static createRationalFromString(rationalString: string): RationalNumber | 基于给定的字符串创建一个RationalNumber对象。 |
| compareTo(another: RationalNumber): number | 将当前的RationalNumber对象与给定的对象进行比较。 |
| equals(obj: object): number | 检查给定对象是否与当前 RationalNumber 对象相同。 |
| valueOf(): number | 将当前的RationalNumber对象进行取整数值或者浮点数值。 |
| static getCommonDivisor(number1: number, number2: number,): number | 获得两个指定数的最大公约数。 |
| getDenominator(): number | 获取当前的RationalNumber对象的分母。 |
| getNumerator(): number | 获取当前的RationalNumber对象的分子。 |
| isFinite(): boolean | 检查当前的RationalNumber对象是有限的。 |
| isNaN(): boolean | 检查当前RationalNumber对象是否表示非数字(NaN)值。 |
| isZero(): boolean | 检查当前RationalNumber对象是否表示零值。 |
| toString(): string | 获取当前RationalNumber对象的字符串表示形式。 |
| constructor(capacity?: number) | 创建默认构造函数用于创建一个新的LruBuffer实例，默认容量为64。 |
| updateCapacity(newCapacity: number): void | 将缓冲区容量更新为指定容量，如果 newCapacity 小于或等于 0，则抛出此异常。 |
| toString(): string | 返回对象的字符串表示形式，输出对象的字符串表示  |
| values(): V[] | 获取当前缓冲区中所有值的列表，输出按升序返回当前缓冲区中所有值的列表，从最近访问到最近最少访问。 |
| length: number | 代表当前缓冲区中值的总数，输出返回当前缓冲区中值的总数。 |
| getCapacity(): number | 获取当前缓冲区的容量，输出返回当前缓冲区的容量。 |
| clear(): void | 从当前缓冲区清除键值对，清除键值对后，调用afterRemoval()方法依次对其执行后续操作。 |
| getCreateCount(): number | 获取createDefault()返回值的次数,输出返回createDefault()返回值的次数。 |
| getMissCount(): number | 获取查询值不匹配的次数，输出返回查询值不匹配的次数。 |
| getRemovalCount(): number | 获取从缓冲区中逐出值的次数，输出从缓冲区中驱逐的次数。 |
| getMatchCount​(): number | 获取查询值匹配成功的次数，输出返回查询值匹配成功的次数。 |
| getPutCount(): number | 获取将值添加到缓冲区的次数，输出返回将值添加到缓冲区的次数。 |
| isEmpty(): boolean | 检查当前缓冲区是否为空，输出如果当前缓冲区不包含任何值，则返回 true 。 |
| get(key: K) : V \| undefined | 表示要查询的键，输出如果指定的键存在于缓冲区中，则返回与键关联的值；否则返回undefined。 |
| put(key: K , value: V): V | 将键值对添加到缓冲区，输出与添加的键关联的值；如果要添加的键已经存在，则返回原始值，如果键或值为空，则抛出此异常。 |
| keys(): K[ ] | 获取当前缓冲区中值的键列表，输出返回从最近访问到最近最少访问排序的键列表。 |
| remove​(key: K): V \| undefined |  从当前缓冲区中删除指定的键及其关联的值。 |
| afterRemoval(isEvict: boolean, key: K, value : V, newValue : V):void | 删除值后执行后续操作。 |
| contains(key: K): boolean | 检查当前缓冲区是否包含指定的键，输出如果缓冲区包含指定的键，则返回 true 。 |
| createDefault(key: K): V | 如果未计算特定键的值，则执行后续操作，参数表示丢失的键,输出返回与键关联的值。 |
| entries(): [K,V] | 允许迭代包含在这个对象中的所有键值对。每对的键和值都是对象。 |
| \[Symbol.iterator\](): [K,V] | 返回以键值对得形式得一个二维数组。 |
| constructor(lowerObj: ScopeType, upperObj : ScopeType) | 创建并返回一个Scope对象，用于创建指定下限和上限的作用域实例的构造函数。 |
| toString(): string | 该字符串化方法返回一个包含当前范围的字符串表示形式。 |
| intersect(range: Scope): Scope | 获取给定范围和当前范围的交集。 |
| intersect(lowerObj: ScopeType, upperObj: ScopeType): Scope | 获取当前范围与给定下限和上限范围的交集。 |
| getUpper(): ScopeType | 获取当前范围的上限。 |
| getLower(): ScopeType | 获取当前范围的下限。 |
| expand(lowerObj: ScopeType, upperObj:  ScopeType): Scope | 创建并返回包括当前范围和给定下限和上限的并集。 |
| expand(range: Scope): Scope | 创建并返回包括当前范围和给定范围的并集。 |
| expand(value: ScopeType): Scope | 创建并返回包括当前范围和给定值的并集。 |
| contains(value: ScopeType): boolean | 检查给定value是否包含在当前范围内。 |
| contains(range: Scope): boolean | 检查给定range是否在当前范围内。 |
| clamp(value: ScopeType): ScopeType | 将给定value限定到当前范围内。 |
| function printf(format: string, ...args: Object[]): string | printf()方法使用第一个参数作为格式字符串（其可以包含零个或多个格式说明符）来返回格式化的字符串。 |
| function getErrorString(errno: number): string | getErrorString()方法使用一个系统的错误数字作为参数，用来返回系统的错误信息。 |
| function callbackWrapper(original: Function): (err: Object, value: Object) => void | 参数为一个采用 async 函数（或返回 Promise 的函数）并返回遵循错误优先回调风格的函数，即将 (err, value) => ... 回调作为最后一个参数。 在回调中，第一个参数将是拒绝原因（如果 Promise 已解决，则为 null），第二个参数将是已解决的值。 |
| function promiseWrapper(original: (err: Object, value: Object) => void): Object | 参数为采用遵循常见的错误优先的回调风格的函数（也就是将 (err, value) => ... 回调作为最后一个参数），并返回一个返回 promise 的版本。 |
| isAnyArrayBuffer(value: Object): boolean | 检查输入的value是否是ArrayBuffer或SharedArrayBuffer类型。 |
| isArrayBufferView(value: Object): boolean | 检查输入的value是否是napi_int8_array或napi_uint8_array或napi_uint8_clamped_array或napi_int16_array或napi_uint16_array或napi_int32_array或napi_uint32_array或napi_float32_array或napi_float64_array数组或DataView类型。 |
| isArgumentsObject(value: Object): boolean | 检查输入的value是否是一个arguments对象类型。 |
| isArrayBuffer(value: Object): boolean | 检查输入的value是否是ArrayBuffer类型。 |
| isAsyncFunction(value: Object): boolean | 检查输入的value是否是异步函数类型。 |
| isBigInt64Array(value: Object): boolean | 检查输入的value是否是BigInt64Array数组类型。 |
| isBigUint64Array(value: Object): boolean | 检查输入的value是否是BigUint64Array数组类型。 |
| isBooleanObject(value: Object): boolean | 检查输入的value是否是一个布尔对象类型。 |
| isBoxedPrimitive(value: Object): boolean | 检查输入的value是否是Boolean或Number或String或Symbol对象类型。 |
| isDataView(value: Object): boolean | 检查输入的value是否是DataView类型。 |
| isDate(value: Object): boolean | 检查输入的value是否是Date类型。 |
| isExternal(value: Object): boolean | 检查输入的value是否是一个native External值类型。 |
| isFloat32Array(value: Object): boolean | 检查输入的value是否是Float32Array数组类型。 |
| isFloat64Array(value: Object): boolean | 检查输入的value是否是Float64Array数组类型。 |
| isGeneratorFunction(value: Object): boolean | 检查输入的value是否是一个generator函数类型。 |
| isGeneratorObject(value: Object): boolean | 检查输入的value是否是一个generator对象类型。 |
| isInt8Array(value: Object): boolean | 检查输入的value是否是Int8Array数组类型。 |
| isInt16Array(value: Object): boolean | 检查输入的value是否是Int16Array数组类型。 |
| isInt32Array(value: Object): boolean | 检查输入的value是否是Int32Array数组类型。 |
| isMap(value: Object): boolean | 检查输入的value是否是Map类型。 |
| isMapIterator(value: Object): boolean | 检查输入的value是否是Map的iterator类型。 |
| isModuleNamespaceObject(value: Object): boolean | 检查输入的value是否是Module Namespace Object对象类型。 |
| isNativeError(value: Object): boolean | 检查输入的value是否是Error类型。 |
| isNumberObject(value: Object): boolean | 检查输入的value是否是Number对象类型。 |
| isPromise(value: Object): boolean | 检查输入的value是否是Promise类型。 |
| isProxy(value: Object): boolean | 检查输入的value是否是Proxy类型。 |
| isRegExp(value: Object): boolean | 检查输入的value是否是RegExp类型。 |
| isSet(value: Object): boolean | 检查输入的value是否是Set类型。 |
| isSetIterator(value: Object): boolean | 检查输入的value是否是Set的iterator类型。 |
| isSharedArrayBuffer(value: Object): boolean | 检查输入的value是否是SharedArrayBuffer类型。 |
| isStringObject(value: Object): boolean | 检查输入的value是否是一个String对象类型。 |
| isSymbolObject(value: Object): boolean | 检查输入的value是否是一个Symbol对象类型。 |
| isTypedArray(value: Object): boolean | 检查输入的value是否是TypedArray包含的类型。 |
| isUint8Array(value: Object): boolean | 检查输入的value是否是Uint8Array数组类型。 |
| isUint8ClampedArray(value: Object): boolean | 检查输入的value是否是Uint8ClampedArray数组类型。 |
| isUint16Array(value: Object): boolean | 检查输入的value是否是Uint16Array数组类型。 |
| isUint32Array(value: Object): boolean | 检查输入的value是否是Uint32Array数组类型。 |
| isWeakMap(value: Object): boolean | 检查输入的value是否是WeakMap类型。 |
| isWeakSet(value: Object): boolean | 检查输入的value是否是WeakSet类型。 |
| add(element: T): boolean | 在ArrayList尾部插入元素。 |
| insert(element: T, index: number): void | 在长度范围内任意位置插入指定元素。 |
| has(element: T): boolean | 判断此ArrayList中是否含有该指定元素。 |
| getIndexOf(element: T): number | 返回指定元素第一次出现时的下标值，查找失败返回-1。 |
| removeByIndex(index: number): T | 根据元素的下标值查找元素，返回元素后将其删除。 |
| remove(element: T): boolean | 删除查找到的第一个指定的元素。 |
| getLastIndexOf(element: T): number | 返回指定元素最后一次出现时的下标值，查找失败返回-1。 |
| removeByRange(fromIndex: number, toIndex: number): void | 从一段范围内删除元素，包括起始值但不包括终止值。 |
| replaceAllElements(callbackFn: (value: T, index?: number, arrlist?: ArrayList<T>) => T,
thisArg?: Object): void | 用户操作ArrayList中的元素，用操作后的元素替换原元素并返回操作后的元素。 |
| forEach(callbackFn: (value: T, index?: number, arrlist?: ArrayList<T>) => void,
  thisArg?: Object): void | 通过回调函数来遍历ArrayList实例对象上的元素以及元素对应的下标。 |
| sort(comparator?: (firstValue: T, secondValue: T) => number): void | 对ArrayList中的元素排序。 |
| subArrayList(fromIndex: number, toIndex: number): ArrayList<T> | 根据下标截取ArrayList中的一段元素，并返回这一段ArrayList实例，包括起始值但不包括终止值。 |
| clear(): void | 清除ArrayList中的所有元素，并把length置为0。 |
| clone(): ArrayList<T> | 克隆一个与ArrayList相同的实例，并返回克隆后的实例。修改克隆后的实例并不会影响原实例。 |
| getCapacity(): number | 获取当前实例的容量大小。 |
| convertToArray(): Array<T> | 把当前ArrayList实例转换成数组，并返回转换后的数组。 |
| isEmpty(): boolean | 判断该ArrayList是否为空。 |
| increaseCapacityTo(newCapacity: number): void | 如果传入的新容量大于或等于ArrayList中的元素个数，将容量变更为新容量。 |
| trimToCurrentLength(): void | 把容量限制为当前的length大小。 |
| insertFront(element: T): void | 在deque头部插入元素。 |
| insertEnd(element: T): void | 在deque尾部插入元素。 |
| has(element: T): boolean | 判断此Deque中是否含有该指定元素。 |
| getFirst(): T | 获取Deque实例中的头元素。 |
| getLast(): T | 获取Deque实例中的尾元素。 |
| popFirst(): T | 删除并返回双端队列的首元素。 |
| popLast(): T | 删除并返回双端队列的尾元素。 |
| forEach(callbackFn: (value: T, index?: number, deque?: Deque<T>) => void,thisArg?: Object): void |
通过回调函数来遍历Deque实例对象上的元素以及元素对应的下标。 |
| isEmpty(): boolean | 判断该HashMap是否为空。 |
| hasKey(key: K): boolean | 判断此HashMap中是否含有该指定key。 |
| hasValue(value: V): boolean | 判断此HashMap中是否含有该指定value。 |
| get(key: K): V | 获取指定key所对应的value。 |
| setAll(map: HashMap<K, V>): void | 将一个HashMap中的所有元素组添加到另一个hashMap中。 |
| set(key: K, value: V): Object | 向HashMap中添加一组数据。 |
| remove(key: K): V | 删除指定key所对应元素。 |
| clear(): void | 清除HashMap中的所有元素,并把length置为0。 |
| keys(): IterableIterator<K> | 返回包含此映射中包含的键名的新迭代器对象。 |
| values(): IterableIterator<V> | 返回包含此映射中包含的键值的新迭代器对象。 |
| replace(key: K, newValue: V): boolean | 对HashMap中一组数据进行更新（替换）。 |
| forEach(callbackFn: (value?: V, key?: K, map?: HashMap<K, V>) => void,thisArg?: Object): void |
通过回调函数来遍历HashMap实例对象上的元素以及元素对应的下标。 |
| entries(): IterableIterator<[K, V]> | 返回包含此映射中包含的键值对的新迭代器对象。 |
| isEmpty(): boolean | 判断该HashSet是否为空。 |
| has(value: T): boolean | 判断此HashSet中是否含有该指定元素。 |
| add(value: T): boolean | 向HashSet中添加数据。 |
| remove(value: T): boolean | 删除指定的元素。 |
| clear(): void | 清除HashSet中的所有元素,并把length置为0。 |
| forEach(callbackFn: (value?: T, key?: T, set?: HashSet<T>) => void,thisArg?: Object): void |
通过回调函数来遍历实例对象上的元素以及元素对应的下标。 |
| values(): IterableIterator<T> | 返回包含此映射中包含的键值的新迭代器对象。 |
| entries(): IterableIterator<[T, T]> | 返回包含此映射中包含的键值对的新迭代器对象。 |
| hasAll(map: LightWeightMap<K, V>): boolean | 判断此LightWeightMap中是否含有该指定map中的所有元素。 |
| hasKey(key: K): boolean | 判断此LightWeightMap中是否含有该指定key。 |
| hasValue(value: V): boolean | 判断此LightWeightMap中是否含有该指定value。 |
| increaseCapacityTo(minimumCapacity: number): void | 将当前LightWeightMap扩容至可以容纳指定数量元素。 |
| entries(): IterableIterator<[K, V]> | 返回包含此映射中包含的键值对的新迭代器对象。 |
| get(key: K): V | 获取指定key所对应的value。 |
| getIndexOfKey(key: K): number | 查找key元素第一次出现的下标值，如果没有找到该元素返回-1。 |
| getIndexOfValue(value: V): number | 查找value元素第一次出现的下标值，如果没有找到该元素返回-1。 |
| isEmpty(): boolean | 判断该LightWeightMap是否为空。 |
| getKeyAt(index: number): K | 查找指定下标的元素键值对中key值，否则返回undefined。 |
| keys(): IterableIterator<K> | 返回包含此映射中包含的键的新迭代器对象。 |
| setAll(map: LightWeightMap<K, V>): void | 将一个LightWeightMap中的所有元素组添加到另一个lightWeightMap中。 |
| set(key: K, value: V): Object | 向LightWeightMap中添加一组数据。 |
| remove(key: K): V | 删除并返回指定key映射的元素。 |
| removeAt(index: number): boolean | 删除指定下标对应的元素。 |
| clear(): void | 清除LightWeightMap中的所有元素,并把length置为0。 |
| setValueAt(index: number, newValue: V): boolean | 替换指定下标对应键值对中的元素。 |
| forEach(callbackFn: (value?: V, key?: K, map?: LightWeightMap<K, V>) => void,thisArg?: Object): void |
通过回调函数来遍历实例对象上的元素以及元素对应的下标。 |
| [Symbol.iterator](): IterableIterator<[K, V]> | 返回一个迭代器，迭代器的每一项都是一个 JavaScript 对象,并返回该对象。 |
| toString(): String | 将此映射中包含的键值对拼接成字符串，并返回字符串类型。 |
| getValueAt(index: number): V | 获取指定下标对应键值对中的元素。 |
| values(): IterableIterator<V> | 返回包含此映射中包含的键值的新迭代器对象。 |
| add(obj: T): boolean | 向此容器中添加数据。 |
| addAll(set: LightWeightSet<T>): boolean | 将另一个容器中的所有元素组添加到当前容器中。 |
| hasAll(set: LightWeightSet<T>): boolean | 判断此容器中是否含有该指定set中的所有元素。 |
| has(key: T): boolean | 判断此容器中是否含有该指定key。 |
| equal(obj: Object): boolean | 判断此容器中是否含有该指定obj同类型的对象。 |
| increaseCapacityTo(minimumCapacity: number): void | 将当前容器扩容至可以容纳指定数量元素。 |
| getIndexOf(key: T): number | 获取指定key所对应的下标。 |
| remove(key: T): T | 删除并返回指定key对应的元素。 |
| removeAt(index: number): boolean | 删除指定下标所对应的元素。 |
| clear(): void | 清除容器中的所有元素，并把length置为0。 |
| forEach(callbackFn: (value?: T, key?: T, set?: LightWeightSet<T>) => void,thisArg?: Object): void |
通过回调函数来遍历LightWeightSet实例对象上的元素以及元素对应的下标。 |
| [Symbol.iterator](): IterableIterator<T> | 返回一个迭代器，迭代器的每一项都是一个 JavaScript 对象，并返回该对象。 |
| toString(): String | 获取包含容器中所有键和值的字符串。 |
| toArray(): Array<T> | 获取包含此容器中所有对象的数组。 |
| getValueAt(index: number): T | 获取此容器中指定下标对应的元素。 |
| values(): IterableIterator<T> | 返回包含此映射中包含的键值的新迭代器对象。 |
| entries(): IterableIterator<[T, T]> | 返回包含此映射中包含的键值对的新迭代器对象。 |
| isEmpty(): boolean | 判断该容器是否为空。 |
| add(element: T): boolean | 在LinkedList尾部插入元素。 |
| insert(index: number, element: T): void | 在长度范围内任意插入指定元素。 |
| get(index: number): T | 根据下标获取LinkedList中的元素。 |
| addFirst(element: T): void | 在LinkedList头部插入元素。 |
| removeFirst(): T | 删除并返回LinkedList的第一个元素。 |
| removeLast(): T | 删除并返回LinkedList的最后一个元素。 |
| has(element: T): boolean | 判断此LinkedList中是否含有该指定元素。 |
| getIndexOf(element: T): number | 返回指定元素第一次出现时的下标值，查找失败返回-1。 |
| removeByIndex(index: number): T | 根据元素的下标值查找元素，返回元素后将其删除。 |
| remove(element: T): boolean | 删除查找到的第一个指定的元素。 |
| removeFirstFound(element: T): boolean | 删除第一次出现的指定元素。 |
| removeLastFound(element: T): boolean | 删除最后一次出现的指定元素。 |
| getLastIndexOf(element: T): number | 返回指定元素最后一次出现时的下标值，查找失败返回-1。 |
| getFirst(): T | 获取LinkedList实例中的第一个元素。 |
| getLast(): T | 获取LinkedList实例中的最后一个元素。 |
| set(index: number, element: T): T | 将此LinkedList中指定位置的元素替换为指定元素。 |
| forEach(callbackFn: (value: T, index?: number, LinkedList?: LinkedList<T>) => void,thisArg?: Object): void |
通过回调函数来遍历LinkedList实例对象上的元素以及元素对应的下标。 |
| clear(): void | 清除LinkedList中的所有元素，并把length置为0。 |
| clone(): LinkedList<T> | 克隆一个与LinkedList相同的实例，并返回克隆后的实例。修改克隆后的实例并不会影响原实例。 |
| convertToArray(): Array<T> | 把当前LinkedList实例转换成数组，并返回转换后的数组。 |
| [Symbol.iterator](): IterableIterator<T> | 返回一个迭代器，迭代器的每一项都是一个 JavaScript 对象，并返回该对象。 |
| add(element: T): boolean | 在List尾部插入元素。 |
| insert(element: T, index: number): void | 在长度范围内任意位置插入指定元素。 |
| get(index: number): T | 根据下标获取List中的元素。 |
| has(element: T): boolean | 判断此List中是否含有该指定元素。 |
| getIndexOf(element: T): number | 查找指定元素第一次出现的下标值，查找失败返回-1。 |
| removeByIndex(index: number): T | 根据元素的下标值查找元素，返回元素后将其删除。 |
| remove(element: T): boolean | 删除查找到的第一个指定的元素。 |
| getLastIndexOf(element: T): number | 查找指定元素最后一次出现的下标值，查找失败返回-1。 |
| getFirst(): T | 获取List实例中的第一个元素。 |
| getLast(): T | 获取List实例中的最后一个元素。 |
| set(index: number, element: T): T | 将此 List 中指定位置的元素替换为指定元素。 |
| equal(obj: Object): boolean | 比较指定对象与此List是否相等。 |
| forEach(callbackFn: (value: T, index?: number, List?: List<T>) => void,thisArg?: Object): void |
通过回调函数来遍历List实例对象上的元素以及元素对应的下标。 |
| sort(comparator: (firstValue: T, secondValue: T) => number): void | 对List中的元素进行一个排序操作。 |
| clear(): void | 清除List中的所有元素，并把length置为0。 |
| getSubList(fromIndex: number, toIndex: number): List<T> |
根据下标截取List中的一段元素，并返回这一段List实例，包括起始值但不包括终止值。 |
| replaceAllElements(callbackFn: (value: T, index?: number, list?: List<T>) => T,thisArg?: Object): void;T |
用户操作List中的元素,用操作后的元素替换原元素并返回操作后的元素。 |
| convertToArray(): Array<T> | 把当前List实例转换成数组，并返回转换后的数组。 |
| isEmpty(): boolean | 判断该List是否为空。 |
| [Symbol.iterator](): IterableIterator<T> | 返回一个迭代器，迭代器的每一项都是一个 JavaScript 对象，并返回该对象。 |
| add(key: number, value: T): void | 向容器中添加一组数据。 |
| clear(): void | 清除容器中的所有元素，并把length置为0。 |
| clone(): PlainArray<T> | 克隆一个实例，并返回克隆后的实例。修改克隆后的实例并不会影响原实例。 |
| has(key: number): boolean | 判断此容器中是否含有该指定key。 |
| get(key: number): T | 获取指定key所对应的value。 |
| getIndexOfKey(key: number): number | 查找指定key第一次出现的下标值，如果没有找到该key返回-1。 |
| getIndexOfValue(value: T): number | 查找指定value元素第一次出现的下标值，如果没有找到该value元素返回-1。 |
| isEmpty(): boolean | 判断该容器是否为空。 |
| getKeyAt(index: number): number | 查找指定下标元素键值对中的key值。 |
| remove(key: number): T | 删除指定key对应元素。 |
| removeAt(index: number): T | 删除指定下标对应的元素。 |
| removeRangeFrom(index: number, size: number): number | 删除一定范围内的元素。 |
| setValueAt(index: number, value: T): void | 替换容器中指定下标对应键值对中的键值。 |
| toString(): String | 获取包含容器中所有键和值的字符串。 |
| getValueAt(index: number): T | 查找指定下标元素键值对中的Value值，失败返回undefined。 |
| forEach(callbackFn: (value: T, index?: number, PlainArray?: PlainArray<T>) => void,thisArg?: Object): void |
通过回调函数来遍历实例对象上的元素以及元素对应的下标。 |
| [Symbol.iterator](): IterableIterator<[number, T]> | 返回一个包含key-value键值对的迭代器对象，其中key是number类型。 |
| add(element: T): boolean | 在队列尾部插入元素。 |
| getFirst(): T | 获取队列的头元素。 |
| pop(): T | 删除头元素并返回该删除元素。 |
| forEach(callbackFn: (value: T, index?: number, Queue?: Queue<T>) => void, thisArg?: Object): void; |
通过回调函数来遍历Queue实例对象上的元素以及元素对应的下标。 |
| [Symbol.iterator](): IterableIterator<T> | 返回一个迭代器，迭代器的每一项都是一个 JavaScript 对象,并返回该对象。 |
| isEmpty(): boolean | 判断该栈是否为空。 |
| peek(): T | 获取并返回栈顶元素。 |
| pop(): T | 删除栈顶元素并返回该删除元素。 |
| push(item: T): T | 在栈顶插入元素，并返回该元素。 |
| locate(element: T): number | 返回指定元素第一次出现时的下标值，查找失败返回-1。 |
| forEach(callbackFn: (value: T, index?: number, stack?: Stack<T>) => void,thisArg?: Object): void |
通过回调函数来遍历Stack实例对象上的元素以及元素对应的下标。 |
| [Symbol.iterator](): IterableIterator<T> | 返回一个迭代器，迭代器的每一项都是一个 JavaScript 对象,并返回该对象。 |
| isEmpty(): boolean | 判断该容器是否为空。 |
| hasKey(key: K): boolean | 判断此容器中是否含有该指定key。 |
| hasValue(value: V): boolean | 判断此容器中是否含有该指定value。 |
| get(key: K): V | 获取指定key所对应的value。 |
| getFirstKey(): K | 获取容器中排序第一的key。 |
| getLastKey(): K | 获取容器中排序最后的key。 |
| setAll(map: TreeMap<K, V>): void | 将一个TreeMap中的所有元素组添加到另一个TreeMap中。 |
| set(key: K, value: V): Object | 向容器中添加一组数据。 |
| remove(key: K): V | 删除指定key对应的元素。 |
| clear(): void | 清除容器中的所有元素，并把length置为0。 |
| getLowerKey(key: K): K | 获取容器中比传入key排序靠前一位的key。 |
| getHigherKey(key: K): K | 获取容器中比传入key排序靠后一位的key。 |
| keys(): IterableIterator<K> | 返回包含此映射中包含的键的新迭代器对象。 |
| values(): IterableIterator<V> | 返回包含此映射中键值对的新迭代器对象。 |
| replace(key: K, newValue: V): boolean | 对容器中一组数据进行更新（替换）。 |
| forEach(callbackFn: (value?: V, key?: K, map?: TreeMap<K, V>) => void, thisArg?: Object): void; |
通过回调函数来遍历实例对象上的元素以及元素对应的下标。 |
| entries(): IterableIterator<[K, V]> | 返回包含此映射中键值对的新迭代器对象。 |
| [Symbol.iterator](): IterableIterator<[K, V]> | 返回一个迭代器，迭代器的每一项都是一个JavaScript对象，并返回该对象。 |
| isEmpty(): boolean | 判断该容器是否为空。 |
| has(value: T): boolean | 判断此容器中是否含有该指定元素。 |
| add(value: T): boolean | 向容器中添加一组数据。 |
| remove(value: T): boolean | 删除指定的元素。 |
| clear(): void | 清除容器中的所有元素，并把length置为0。 |
| getFirstValue(): T | 获取容器中排序第一的数据。 |
| getLastValue(): T | 获取容器中排序最后的数据。 |
| getLowerValue(key: T): T | 获取容器中比传入元素排序靠前一位的元素。 |
| getHigherValue(key: T): T | 获取容器中比传入元素排序靠后一位的元素。 |
| popFirst(): T | 删除容器中排序最前的数据。 |
| popLast(): T | 删除容器中排序最后的数据。 |
| forEach(callbackFn: (value?: T, key?: T, set?: TreeSet<T>) => void, thisArg?: Object): void; |
通过回调函数来遍历实例对象上的元素以及元素对应的下标。 |
| values(): IterableIterator<T> | 返回包含此映射中键值对的新迭代器对象。 |
| entries(): IterableIterator<[T, T]> | 返回包含此映射中键值对的新迭代器对象。 |
| [Symbol.iterator](): IterableIterator<T> | 返回一个迭代器，迭代器的每一项都是一个JavaScript对象，并返回该对象。 |

printf中每个说明符都替换为来自相应参数的转换后的值。 支持的说明符有:
| 式样化字符 | 式样要求 |
| -------- | -------- |
|    %s:  | String 将用于转换除 BigInt、Object 和 -0 之外的所有值。|
|    %d:  | Number 将用于转换除 BigInt 和 Symbol 之外的所有值。|
|    %i:  | parseInt(value, 10) 用于除 BigInt 和 Symbol 之外的所有值。|
|    %f:  | parseFloat(value) 用于除 Symbol 之外的所有值。|
|    %j:  | JSON。 如果参数包含循环引用，则替换为字符串 '[Circular]'。|
|    %o:  | Object. 具有通用 JavaScript 对象格式的对象的字符串表示形式。类似于具有选项 { showHidden: true, showProxy: true } 的 util.inspect()。这将显示完整的对象，包括不可枚举的属性和代理。|
|    %O:  | Object. 具有通用 JavaScript 对象格式的对象的字符串表示形式。类似于没有选项的 util.inspect()。 这将显示完整的对象，但不包括不可枚举的属性和代理。|
|    %c:  | 此说明符被忽略，将跳过任何传入的 CSS 。|
|    %%:  | 单个百分号 ('%')。 这不消耗待式样化参数。|

#### 2.3.2. 使用说明
各接口使用方法如下：

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
var result1 = res.valueOf();
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
var res = pro.valueOf();
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
pro .put(2,10);
var result = pro[symbol.iterator]();
```
51.afterRemoval()
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
Scope接口中构造新类，实现compareTo方法。

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
103.add()
```
import ArrayList from "@ohos.util.ArrayList"
let arrayList = new ArrayList();
let result = arrayList.add(8);
```
104.insert()
```
import ArrayList from "@ohos.util.ArrayList"
let arrayList = new ArrayList();
arrayList.add("四");
arrayList.add("三");
arrayList.add(1);
arrayList.add("a");
arrayList.insert(8, 2);
```
105.add()
```
import ArrayList from "@ohos.util.ArrayList"
let arrayList = new ArrayList();
arrayList.add("四");
arrayList.add("三");
arrayList.add(1);
arrayList.add("a");
arrayList.insert(8, 2);
let result = arrayList.has(8);
```
106.getIndexOf()
```
import ArrayList from "@ohos.util.ArrayList"
let arrayList = new ArrayList();
arrayList.add(1);
arrayList.add("三");
arrayList.add(1);
arrayList.add("a");
arrayList.insert(1, 2);
let result = arrayList.getIndexOf(1);
```
107.removeByIndex()
```
import ArrayList from "@ohos.util.ArrayList"
let arrayList = new ArrayList();
arrayList.add(1);
arrayList.add("三");
arrayList.add(1);
arrayList.add("a");
let result = arrayList.removeByIndex(2);
```
108.remove()
```
import ArrayList from "@ohos.util.ArrayList"
let arrayList = new ArrayList();
arrayList.add("四");
arrayList.add("三");
arrayList.add(1);
arrayList.add("a");
let result = arrayList.remove("三");
```
109.getLastIndexOf()
```
import ArrayList from "@ohos.util.ArrayList"
let arrayList = new ArrayList();
arrayList.add("四");
arrayList.add("三");
arrayList.add(1);
arrayList.add("a");
arrayList.add(1);
arrayList.add("b");
let result = arrayList.getLastIndexOf(1);
```
110.removeByRange()
```
import ArrayList from "@ohos.util.ArrayList"
let arrayList = new ArrayList();
arrayList.add("四");
arrayList.add("三");
arrayList.add("a");
arrayList.add("b");
arrayList.add("c");
arrayList.removeByRange(1, 3);
```
111.replaceAllElements()
```
import ArrayList from "@ohos.util.ArrayList"
let arrayList = new ArrayList();
arrayList.add(4);
arrayList.add(3);
arrayList.add(1);
arrayList.add(2);
arrayList.add(14);
arrayList.replaceAllElements((item, index) => {
    return (item = 2 * item);
});
```
112.forEach()
```
import ArrayList from "@ohos.util.ArrayList"
let arrayList = new ArrayList();
arrayList.add(4);
arrayList.add(3);
arrayList.add(1);
arrayList.add(2);
arrayList.add(14);
let arr = [];
arrayList.forEach((item, index) => {
    arr.push(item);
});
```
113.sort()
```
import ArrayList from "@ohos.util.ArrayList"
let arrayList = new ArrayList();
arrayList.add(4);
arrayList.add(3);
arrayList.add(1);
arrayList.add(2);
arrayList.add(14);
arrayList.sort((a, b) => a - b);
```
114.subArrayList()
```
import ArrayList from "@ohos.util.ArrayList"
let arrayList = new ArrayList();
arrayList.add(4);
arrayList.add(3);
arrayList.add(1);
arrayList.add(2);
arrayList.add(14);
let result = arrayList.subArrayList(0, 5);
```
115.clear()
```
import ArrayList from "@ohos.util.ArrayList"
let arrayList = new ArrayList();
arrayList.add(4);
arrayList.add(3);
arrayList.add(1);
arrayList.add(2);
arrayList.add(14);
arrayList.clear();
```
116.clone()
```
import ArrayList from "@ohos.util.ArrayList"
let arrayList = new ArrayList();
arrayList.add(4);
arrayList.add(3);
arrayList.add(1);
arrayList.add(2);
arrayList.add(14);
let res = arrayList.clone();
```
117.getCapacity()
```
import ArrayList from "@ohos.util.ArrayList"
let arrayList = new ArrayList();
arrayList.add(4);
arrayList.add(3);
arrayList.add(1);
arrayList.add(2);
arrayList.add(14);
let result = arrayList.getCapacity();
```
118.convertToArray()
```
import ArrayList from "@ohos.util.ArrayList"
let arrayList = new ArrayList();
arrayList.add(4);
arrayList.add(3);
arrayList.add(1);
arrayList.add(2);
arrayList.add(14);
let arr = arrayList.convertToArray();
```
119.isEmpty()
```
import ArrayList from "@ohos.util.ArrayList"
let arrayList = new ArrayList();
arrayList.add(4);
arrayList.add(3);
arrayList.add(1);
arrayList.clear();
let result = arrayList.isEmpty();
```
120.increaseCapacityTo()
```
import ArrayList from "@ohos.util.ArrayList"
let arrayList = new ArrayList();
arrayList.add(4);
arrayList.add(3);
arrayList.add(1);
arrayList.add(2);
arrayList.add(14);
arrayList.increaseCapacityTo(-1);
```
121.trimToCurrentLength()
```
import ArrayList from "@ohos.util.ArrayList"
let arrayList = new ArrayList();
arrayList.add(4);
arrayList.add(3);
arrayList.add(1);
arrayList.add(2);
arrayList.add(14);
arrayList.trimToCurrentLength();
```
122.insertFront()
```
import Deque from "@ohos.util.Deque"
let deque = new Deque();
deque.insertFront("一");
```
123.insertEnd()
```
import Deque from "@ohos.util.Deque"
let deque = new Deque();
let a = ["a", "b", "c"];
deque.insertEnd(a);
```
124.has()
```
import Deque from "@ohos.util.Deque"
let deque = new Deque();
deque.insertEnd(6);
let result = deque.has(6);
```
125.getFirst()
```
import Deque from "@ohos.util.Deque"
let deque = new Deque();
deque.insertFront("$");
let result = deque.getFirst();
```
126.getLast()
```
import Deque from "@ohos.util.Deque"
let deque = new Deque();
deque.insertFront("$");
let result = deque.getLast();
```
127.popFirst()
```
import Deque from "@ohos.util.Deque"
let deque = new Deque();
deque.insertFront(6);
let result = deque.popFirst();
```
128.popLast()
```
import Deque from "@ohos.util.Deque"
let deque = new Deque();
deque.insertFront(11);
let result = deque.popLast();
```
129.forEach()
```
import Deque from "@ohos.util.Deque"
let deque = new Deque();
deque.insertEnd(1);
deque.insertEnd(2);
deque.insertEnd(3);
let arr = [];
deque.forEach((item, index) => {
    arr.push(item);
});
```
130.isEmpty()
```
import HashMap from "@ohos.util.HashMap"
let hashMap = new HashMap();
hashMap.set(1, "A");
hashMap.set(2, "B");
let result = hashMap.isEmpty();
```
131.hasKey()
```
import HashMap from "@ohos.util.HashMap"
let hashMap = new HashMap();
hashMap.set(1, "A");
hashMap.set(2, "B");
hashMap.set(3, "C");
let result = hashMap.hasKey(3);
```
132.hasValue()
```
import HashMap from "@ohos.util.HashMap"
let hashMap = new HashMap();
hashMap.set(1, "A");
hashMap.set(2, "B");
hashMap.set(3, "C");
let result = hashMap.hasValue("C");
```
133.get()
```
import HashMap from "@ohos.util.HashMap"
let hashMap = new HashMap();
hashMap.set(1, "A");
hashMap.set(2, "B");
hashMap.set(3, "C");
let result = hashMap.get(3);
```
134.setAll()
```
import HashMap from "@ohos.util.HashMap"
let hashMap = new HashMap();
hashMap.set(1, "A");
hashMap.set(2, "B");
hashMap.set(3, "C");
let hashMap1 = new HashMap();
hashMap1.set(1, 1);
hashMap1.set(2, 2);
hashMap.setAll(hashMap1);
```
135.set()
```
import HashMap from "@ohos.util.HashMap"
let hashMap = new HashMap();
hashMap.set(1, "A");
hashMap.set(2, "B");
hashMap.set(3, "C");
```
136.remove()
```
import HashMap from "@ohos.util.HashMap"
let hashMap = new HashMap();
hashMap.set(1, "A");
hashMap.set(2, "B");
hashMap.set(3, "C");
let result = hashMap.remove(3);
```
137.clear()
```
import HashMap from "@ohos.util.HashMap"
let hashMap = new HashMap();
hashMap.set(1, "A");
hashMap.set(2, "B");
hashMap.set(3, "C");
hashMap.clear();
```
138.keys()
```
import HashMap from "@ohos.util.HashMap"
let hashMap = new HashMap();
hashMap.set(1, "A");
hashMap.set(2, "B");
hashMap.set(3, "C");
hashMap.set(4, "D");
hashMap.set(5, "E");
let result = hashMap.keys();
```
139.values()
```
import HashMap from "@ohos.util.HashMap"
let hashMap = new HashMap();
hashMap.set(1, "A");
hashMap.set(2, "B");
hashMap.set(3, "C");
hashMap.set(4, "D");
hashMap.set(5, "E");
let result = hashMap.values();
```
140.replace()
```
import HashMap from "@ohos.util.HashMap"
let hashMap = new HashMap();
hashMap.set(1, "A");
hashMap.set(2, "B");
hashMap.set(3, "C");
hashMap.set(4, "D");
hashMap.set(5, "E");
let result = hashMap.replace(2, "G");
```
141.forEach()
```
import HashMap from "@ohos.util.HashMap"
let hashMap = new HashMap();
hashMap.set(1, "A");
hashMap.set(2, "B");
hashMap.set(3, "C");
hashMap.set(4, "D");
hashMap.set(5, "E");
let vals = [];
let keys = [];
hashMap.forEach((value, key) => {
    keys.push(key);
    vals.push(value);
});
```
142.entries()
```
import HashMap from "@ohos.util.HashMap"
let hashMap = new HashMap();
hashMap.set(1, "A");
hashMap.set(2, "B");
hashMap.set(3, "C");
hashMap.set(4, "D");
hashMap.set(5, "E");
let result = hashMap.entries();
```
143.isEmpty()
```
import HashSet from "@ohos.util.HashSet"
let hashSet = new HashSet();
hashSet.add(1);
hashSet.add(2);
let result = hashSet.isEmpty();
```
144.has()
```
import HashSet from "@ohos.util.HashSet"
let hashSet = new HashSet();
hashSet.add(1);
hashSet.add(2);
let result = hashSet.has(1);
```
145.add()
```
import HashSet from "@ohos.util.HashSet"
let hashSet = new HashSet();
hashSet.add(1);
hashSet.add(2);
```
146.remove()
```
import HashSet from "@ohos.util.HashSet"
let hashSet = new HashSet();
hashSet.add(1);
hashSet.add(2);
let hashSet = hashSet.remove(1);
```
147.clear()
```
import HashSet from "@ohos.util.HashSet"
let hashSet = new HashSet();
hashSet.add(1);
hashSet.add(2);
hashSet.clear();
```
148.forEach()
```
import HashSet from "@ohos.util.HashSet"
let hashSet = new HashSet();
hashSet.add(1);
hashSet.add(2);
hashSet.add(3);
hashSet.add(4);
hashSet.add(5);
let arr = [];
hashSet.forEach((value, index) => {
    arr.push(value);
});
```
149.values()
```
import HashSet from "@ohos.util.HashSet"
let hashSet = new HashSet();
hashSet.add("A");
hashSet.add("B");
hashSet.add("C");
hashSet.add("D");
hashSet.add("E");
let res = hashSet.values();
```
150.entries()
```
import HashSet from "@ohos.util.HashSet"
let hashSet = new HashSet();
hashSet.add(1);
hashSet.add(2);
hashSet.add(3);
let res = hashSet.entries();
```
151.hasAll()
```
import LightWeightMap from "@ohos.util.LightWeightMap"
let lightWeightMap = new LightWeightMap();
lightWeightMap.set("a", "A");
lightWeightMap.set("b", "B");
lightWeightMap.set("c", "C");
lightWeightMap.set("d", "D");
lightWeightMap.set("e", "E");
let lightWeightMap1 = new LightWeightMap();
lightWeightMap1.set("a", "A");
lightWeightMap1.set("d", "D");
let res = lightWeightMap.hasAll(lightWeightMap1);
```
152.hasKey()
```
import LightWeightMap from "@ohos.util.LightWeightMap"
let lightWeightMap = new LightWeightMap();
lightWeightMap.set("a", "A");
lightWeightMap.set("b", "B");
lightWeightMap.set("c", "C");
let res = lightWeightMap.hasKey("a");
```
153.hasValue()
```
import LightWeightMap from "@ohos.util.LightWeightMap"
let lightWeightMap = new LightWeightMap();
lightWeightMap.set("a", "A");
lightWeightMap.set("b", "B");
lightWeightMap.set("c", "C");
let res = lightWeightMap.hasValue("A");
```
154.increaseCapacityTo()
```
import LightWeightMap from "@ohos.util.LightWeightMap"
let lightWeightMap = new LightWeightMap();
lightWeightMap.set("a", "A");
lightWeightMap.set("b", "B");
lightWeightMap.set("c", "C");
lightWeightMap.increaseCapacityTo(3);
```
155.entries()
```
import LightWeightMap from "@ohos.util.LightWeightMap"
let lightWeightMap = new LightWeightMap();
lightWeightMap.set(1, "A");
lightWeightMap.set(2, "B");
lightWeightMap.set(3, "C");
let res = lightWeightMap.entries();
```
156.get()
```
import LightWeightMap from "@ohos.util.LightWeightMap"
let lightWeightMap = new LightWeightMap();
lightWeightMap.set(1, "A");
lightWeightMap.set(2, "B");
lightWeightMap.set(3, "C");
let res = lightWeightMap.get(1);
```
157.getIndexOfKey()
```
import LightWeightMap from "@ohos.util.LightWeightMap"
let lightWeightMap = new LightWeightMap();
lightWeightMap.set(1, "A");
lightWeightMap.set(2, "B");
lightWeightMap.set(3, "C");
let res = lightWeightMap.getIndexOfKey(2);
```
158.getIndexOfValue()
```
import LightWeightMap from "@ohos.util.LightWeightMap"
let lightWeightMap = new LightWeightMap();
lightWeightMap.set(1, "A");
lightWeightMap.set(2, "B");
lightWeightMap.set(3, "C");
lightWeightMap.set(4, "D");
lightWeightMap.set(5, "E");
lightWeightMap.setValueAt(4, "F");
let res = lightWeightMap.getIndexOfValue("F");
```
159.isEmpty()
```
import LightWeightMap from "@ohos.util.LightWeightMap"
let lightWeightMap = new LightWeightMap();
lightWeightMap.set(1, "A");
lightWeightMap.set(2, "B");
let res = lightWeightMap.isEmpty();
```
160.getKeyAt()
```
import LightWeightMap from "@ohos.util.LightWeightMap"
let lightWeightMap = new LightWeightMap();
lightWeightMap.set(1, "A");
lightWeightMap.set(2, "B");
lightWeightMap.set(3, "C");
let res = lightWeightMap.getKeyAt(1);
```
161.keys()
```
import LightWeightMap from "@ohos.util.LightWeightMap"
let lightWeightMap = new LightWeightMap();
lightWeightMap.set(1, "A");
lightWeightMap.set(2, "B");
lightWeightMap.set(3, "C");
let res = lightWeightMap.keys();
```
161.setAll()
```
import LightWeightMap from "@ohos.util.LightWeightMap"
let lightWeightMap = new LightWeightMap();
lightWeightMap.set(1, "A");
lightWeightMap.set(2, "B");
lightWeightMap.set(3, "C");
lightWeightMap.set(4, "D");
lightWeightMap.set(5, "E");
let lightWeightMap1 = new LightWeightMap();
lightWeightMap1.set(6, "A");
lightWeightMap1.set(7, "B");
lightWeightMap.setAll(lightWeightMap1);
```
162.set()
```
import LightWeightMap from "@ohos.util.LightWeightMap"
let lightWeightMap = new LightWeightMap();
lightWeightMap.set(1, "A");
lightWeightMap.set(2, "B");
```
163.remove()
```
import LightWeightMap from "@ohos.util.LightWeightMap"
let lightWeightMap = new LightWeightMap();
lightWeightMap.set(1, "A");
lightWeightMap.set(2, "B");
let res = lightWeightMap.remove(2);
```
164.removeAt()
```
import LightWeightMap from "@ohos.util.LightWeightMap"
let lightWeightMap = new LightWeightMap();
lightWeightMap.set(1, "A");
lightWeightMap.set(2, "B");
let res = lightWeightMap.removeAt(1);
```
165.clear()
```
import LightWeightMap from "@ohos.util.LightWeightMap"
let lightWeightMap = new LightWeightMap();
lightWeightMap.set(1, "A");
lightWeightMap.set(2, "B");
lightWeightMap.clear();
```
166.setValueAt()
```
import LightWeightMap from "@ohos.util.LightWeightMap"
let lightWeightMap = new LightWeightMap();
lightWeightMap.set(1, "A");
lightWeightMap.set(2, "B");
lightWeightMap.setValueAt(0, "a");
```
167.forEach()
```
import LightWeightMap from "@ohos.util.LightWeightMap"
let lightWeightMap = new LightWeightMap();
lightWeightMap.set(1, "A");
lightWeightMap.set(2, "B");
lightWeightMap.set(3, "C");
lightWeightMap.set(4, "D");
lightWeightMap.set(5, "E");
let arr = [];
lightWeightMap.forEach((value, index) => {
    arr.push(value);
});
```
168.[Symbol.iterator]()
```
import LightWeightMap from "@ohos.util.LightWeightMap"
let lightWeightMap = new LightWeightMap();
lightWeightMap.set(1, "A");
lightWeightMap.set(2, "B");
lightWeightMap.set(3, "C");
lightWeightMap.set(4, "D");
lightWeightMap.set(5, "E");
let iters = lightWeightMap[Symbol.iterator]();
```
169.toString()
```
import LightWeightMap from "@ohos.util.LightWeightMap"
let lightWeightMap = new LightWeightMap();
lightWeightMap.set(1, "A");
lightWeightMap.set(2, "B");
lightWeightMap.set(3, "C");
let res = lightWeightMap.toString();
```
170.getValueAt()
```
import LightWeightMap from "@ohos.util.LightWeightMap"
let lightWeightMap = new LightWeightMap();
lightWeightMap.set(1, "A");
lightWeightMap.set(2, "B");
lightWeightMap.set(3, "C");
let res = lightWeightMap.getValueAt(1);
```
171.values()
```
import LightWeightMap from "@ohos.util.LightWeightMap"
let lightWeightMap = new LightWeightMap();
lightWeightMap.set(1, "A");
lightWeightMap.set(2, "B");
lightWeightMap.set(3, "C");
let res = lightWeightMap.values();
```
172.add()
```
import LightWeightSet from "@ohos.util.LightWeightSet"
let lightWeightSet = new LightWeightSet();
lightWeightSet.add(1);
```
173.addAll()
```
import LightWeightSet from "@ohos.util.LightWeightSet"
let lightWeightSet = new LightWeightSet();
lightWeightSet.add(1);
lightWeightSet.add(2);
lightWeightSet.add(3);
lightWeightSet.add(4);
lightWeightSet.add(5);
let lightWeightSet1 = new LightWeightSet();
lightWeightSet1.add(6);
lightWeightSet1.add(7);
let res1 = lightWeightSet.addAll(lightWeightSet1);
```
174.hasAll()
```
import LightWeightSet from "@ohos.util.LightWeightSet"
let lightWeightSet = new LightWeightSet();
lightWeightSet.add("a");
lightWeightSet.add("b");
lightWeightSet.add("c");
lightWeightSet.add("e");
let lightWeightSet1 = new LightWeightSet();
lightWeightSet1.add("a");
lightWeightSet1.add("d");
let res = lightWeightSet.hasAll(lightWeightSet1);
```
175.has()
```
import LightWeightSet from "@ohos.util.LightWeightSet"
let lightWeightSet = new LightWeightSet();
lightWeightSet.add("a");
lightWeightSet.add("b");
lightWeightSet.add("c");
let res = lightWeightSet.has("a");
```
176.equal()
```
import LightWeightSet from "@ohos.util.LightWeightSet"
let lightWeightSet = new LightWeightSet();
lightWeightSet.add(1);
lightWeightSet.add(2);
lightWeightSet.add(3);
let obj = [1,2,3];
let res = lightWeightSet.equal(obj);
```
177.increaseCapacityTo()
```
import LightWeightSet from "@ohos.util.LightWeightSet"
let lightWeightSet = new LightWeightSet();
lightWeightSet.add(1);
lightWeightSet.add(2);
lightWeightSet.add(3);
let res = lightWeightSet.increaseCapacityTo(20);
```
178.getIndexOf()
```
import LightWeightSet from "@ohos.util.LightWeightSet"
let lightWeightSet = new LightWeightSet();
lightWeightSet.add(1);
lightWeightSet.add(2);
lightWeightSet.add(3);
let res = lightWeightSet.getIndexOf(2);
```
179.remove()
```
import LightWeightSet from "@ohos.util.LightWeightSet"
let lightWeightSet = new LightWeightSet();
lightWeightSet.add(1);
lightWeightSet.add(2);
lightWeightSet.add(3);
let res = lightWeightSet.remove(1);
```
180.removeAt()
```
import LightWeightSet from "@ohos.util.LightWeightSet"
let lightWeightSet = new LightWeightSet();
lightWeightSet.add(1);
lightWeightSet.add(2);
lightWeightSet.add(3);
let res = lightWeightSet.removeAt(1);
```
181.clear()
```
import LightWeightSet from "@ohos.util.LightWeightSet"
let lightWeightSet = new LightWeightSet();
lightWeightSet.add(1);
lightWeightSet.add(2);
lightWeightSet.add(3);
lightWeightSet.clear();
```
182.forEach()
```
import LightWeightSet from "@ohos.util.LightWeightSet"
let lightWeightSet = new LightWeightSet();
lightWeightSet.add(1);
lightWeightSet.add(2);
lightWeightSet.add(3);
lightWeightSet.add(4);
lightWeightSet.add(5);
let arr = [];
lightWeightSet.forEach((value, index) => {
    arr.push(value);
});
```
183.[Symbol.iterator]()
```
import LightWeightSet from "@ohos.util.LightWeightSet"
let lightWeightSet = new LightWeightSet();
lightWeightSet.add(1);
lightWeightSet.add(2);
lightWeightSet.add(3);
let res = lightWeightSet[Symbol.iterator]();
```
184.toString()
```
import LightWeightSet from "@ohos.util.LightWeightSet"
let lightWeightSet = new LightWeightSet();
lightWeightSet.add(1);
lightWeightSet.add(2);
lightWeightSet.add(3);
let res = lightWeightSet.toString();
```
185.toArray()
```
import LightWeightSet from "@ohos.util.LightWeightSet"
let lightWeightSet = new LightWeightSet();
lightWeightSet.add(1);
lightWeightSet.add(2);
lightWeightSet.add(3);
let res = lightWeightSet.toArray();
```
186.getValueAt()
```
import LightWeightSet from "@ohos.util.LightWeightSet"
let lightWeightSet = new LightWeightSet();
lightWeightSet.add(1);
lightWeightSet.add(2);
lightWeightSet.add(3);
let res = lightWeightSet.getValueAt(1);
```
187.values()
```
import LightWeightSet from "@ohos.util.LightWeightSet"
let lightWeightSet = new LightWeightSet();
lightWeightSet.add(1);
lightWeightSet.add(2);
lightWeightSet.add(3);
let res = lightWeightSet.values();
```
188.entries()
```
import LightWeightSet from "@ohos.util.LightWeightSet"
let lightWeightSet = new LightWeightSet();
lightWeightSet.add(1);
lightWeightSet.add(2);
lightWeightSet.add(3);
let res = lightWeightSet.entries();
```
189.isEmpty()
```
import LightWeightSet from "@ohos.util.LightWeightSet"
let lightWeightSet = new LightWeightSet();
lightWeightSet.add(1);
lightWeightSet.add(2);
lightWeightSet.add(3);
let res = lightWeightSet.isEmpty();
```
190.add()
```
import LinkedList from "@ohos.util.LinkedList"
let linkedList = new LinkedList();
linkedList.add("四");
```
191.insert()
```
import LinkedList from "@ohos.util.LinkedList"
let linkedList = new LinkedList();
linkedList.add("a");
linkedList.add("b");
linkedList.add("c");
linkedList.add("a");
linkedList.add("b");
let res = linkedList.insert(3, "d");
```
192.get()
```
import LinkedList from "@ohos.util.LinkedList"
let linkedList = new LinkedList();
linkedList.add("a");
linkedList.add("b");
linkedList.add("c");
let res1 = linkedList.get(2);
```
193.addFirst()
```
import LinkedList from "@ohos.util.LinkedList"
let linkedList = new LinkedList();
linkedList.add("a");
linkedList.add("b");
linkedList.add("c");
linkedList.addFirst("e");
```
194.removeFirst()
```
import LinkedList from "@ohos.util.LinkedList"
let linkedList = new LinkedList();
linkedList.add("a");
linkedList.add("b");
linkedList.add("c");
let res = linkedList.removeFirst();
```
195.removeLast()
```
import LinkedList from "@ohos.util.LinkedList"
let linkedList = new LinkedList();
linkedList.add("a");
linkedList.add("b");
linkedList.add("c");
let res = linkedList.removeLast();
```
196.has()
```
import LinkedList from "@ohos.util.LinkedList"
let linkedList = new LinkedList();
linkedList.add("a");
linkedList.add("b");
linkedList.add("c");
let res = linkedList.has(1);
```
197.getIndexOf()
```
import LinkedList from "@ohos.util.LinkedList"
let linkedList = new LinkedList();
linkedList.add("a");
linkedList.add("b");
linkedList.add("c");
let res = linkedList.getIndexOf("b");
```
198.removeByIndex()
```
import LinkedList from "@ohos.util.LinkedList"
let linkedList = new LinkedList();
linkedList.add("a");
linkedList.add("b");
linkedList.add("c");
let res = linkedList.removeByIndex(2);
```
199.remove()
```
import LinkedList from "@ohos.util.LinkedList"
let linkedList = new LinkedList();
linkedList.add("a");
linkedList.add("b");
linkedList.add("c");
let res = linkedList.remove("a");
```
200.removeFirstFound()
```
import LinkedList from "@ohos.util.LinkedList"
let linkedList = new LinkedList();
linkedList.add("a");
linkedList.add("b");
linkedList.add("c");
let res = linkedList.removeFirstFound("b");
```
201.removeLastFound()
```
import LinkedList from "@ohos.util.LinkedList"
let linkedList = new LinkedList();
linkedList.add("a");
linkedList.add("b");
linkedList.add("c");
let res = linkedList.removeLastFound("b");
```
202.getLastIndexOf()
```
import LinkedList from "@ohos.util.LinkedList"
let linkedList = new LinkedList();
linkedList.add("a");
linkedList.add("b");
linkedList.add("c");
linkedList.add("a");
linkedList.add("b");
linkedList.add("b");
let res = linkedList.getLastIndexOf("a");
```
203.getFirst()
```
import LinkedList from "@ohos.util.LinkedList"
let linkedList = new LinkedList();
linkedList.add("a");
linkedList.add("b");
linkedList.add("c");
linkedList.add("a");
linkedList.add("b");
linkedList.add("b");
let res = linkedList.getFirst();
```
204.getLast()
```
import LinkedList from "@ohos.util.LinkedList"
let linkedList = new LinkedList();
linkedList.add("a");
linkedList.add("b");
linkedList.add("c");
linkedList.add("a");
linkedList.add("b");
linkedList.add("b");
let res = linkedList.getLast();
```
205.set()
```
import LinkedList from "@ohos.util.LinkedList"
let linkedList = new LinkedList();
linkedList.add("a");
linkedList.add("b");
linkedList.add("c");
let res = linkedList.set(2, "d");
```
206.forEach()
```
import LinkedList from "@ohos.util.LinkedList"
let linkedList = new LinkedList();
linkedList.add(8);
linkedList.add("一");
linkedList.add("二");
linkedList.add(5);
let c = [1, 2, 3, 4];
linkedList.add(c);
linkedList.add(6);
linkedList.add("三");
linkedList.add("四");
let arr = [];
linkedList.forEach((item, index) => {
    arr.push(item);
});
```
207.clear()
```
import LinkedList from "@ohos.util.LinkedList"
let linkedList = new LinkedList();
linkedList.add("a");
linkedList.add("b");
linkedList.add("c");
let res = linkedList.clear();
```
208.clone()
```
import LinkedList from "@ohos.util.LinkedList"
let linkedList = new LinkedList();
linkedList.add("a");
linkedList.add("b");
linkedList.add("c");
let linkedList1 = linkedList.clone();
```
209.convertToArray()
```
import LinkedList from "@ohos.util.LinkedList"
let linkedList = new LinkedList();
linkedList.add(4);
linkedList.add(3);
linkedList.add(1);
linkedList.add(2);
linkedList.add(14);
let res = linkedList.convertToArray();
```
210.[Symbol.iterator]()
```
import LinkedList from "@ohos.util.LinkedList"
let linkedList = new LinkedList();
linkedList.add("a");
linkedList.add("b");
linkedList.add("c");
linkedList.addFirst("e");
let itr = linkedList[Symbol.iterator]();
```
211.add()
```
import List from "@ohos.util.List"
let list = new List();
list.add("四");
```
212.insert()
```
import List from "@ohos.util.List"
let list = new List();
list.add("a");
list.add("b");
list.add("c");
list.add("a");
list.add("b");
let res = list.insert("d", 3);
```
213.get()
```
import List from "@ohos.util.List"
let list = new List();
list.add("a");
list.add("b");
list.add("c");
list.add("a");
let res = list.set(2, "d");
let res1 = list.get(2);
```
214.has()
```
import List from "@ohos.util.List"
let list = new List();
list.add("a");
list.add("b");
let res = list.has(1);
```
215.getIndexOf()
```
import List from "@ohos.util.List"
let list = new List();
list.add("a");
list.add("b");
list.add("c");
list.add("b");
let res = list.getIndexOf("b");
```
216.removeByIndex()
```
import List from "@ohos.util.List"
let list = new List();
list.add("a");
list.add("b");
list.add("c");
list.add("b");
let res = list.removeByIndex(2);
```
217.remove()
```
import List from "@ohos.util.List"
let list = new List();
list.add("a");
list.add("b");
list.add("c");
list.add("b");
let res = list.remove("a");
```
218.getLastIndexOf()
```
import List from "@ohos.util.List"
let list = new List();
list.add("a");
list.add("b");
list.add("c");
list.add("b");
let res = list.getLastIndexOf("F");
```
219.getFirst()
```
import List from "@ohos.util.List"
let list = new List();
list.add("a");
list.add("b");
list.add("c");
list.add("b");
let res = list.getFirst();
```
220.getLast()
```
import List from "@ohos.util.List"
let list = new List();
list.add("a");
list.add("b");
list.add("c");
list.add("b");
let res = list.getLast();
```
221.set()
```
import List from "@ohos.util.List"
let list = new List();
list.add("a");
list.add("b");
list.add("c");
list.add("a");
let res = list.set(2, "d");
```
222.equal()
```
import List from "@ohos.util.List"
let list = new List();
let list1 = new List();
let res = list.equal(list1);
```
223.forEach()
```
import List from "@ohos.util.List"
let list = new List();
let num = 0;
list.forEach((item, index) => {
    num++;
});
```
224.sort()
```
import List from "@ohos.util.List"
let list = new List();
list.add(4);
list.add(3);
list.add(1);
list.sort((a, b) => a - b);
```
225.clear()
```
import List from "@ohos.util.List"
let list = new List();
list.add(4);
list.add(3);
list.add(1);
list.clear();
```
226.getSubList()
```
import List from "@ohos.util.List"
let list = new List();
list.add(4);
list.add(3);
list.add(1);
list.add(2);
list.add(14);
let res = list.getSubList(2, 4);
```
227.replaceAllElements()
```
import List from "@ohos.util.List"
let list = new List();
let num = 0;
list.replaceAllElements((item, index) => {
    num++;
});
```
228.convertToArray()
```
import List from "@ohos.util.List"
let list = new List();
const res = list.convertToArray();
```
229.isEmpty()
```
import List from "@ohos.util.List"
let list = new List();
let res = list.isEmpty();
```
230.[Symbol.iterator]()
```
import List from "@ohos.util.List"
let list = new List();
list.add("a");
list.add("b");
list.add("c");
let itr = list[Symbol.iterator]();
```
231.add()
```
import PlainArray from "@ohos.util.PlainArray"
let plainArray = new PlainArray();
plainArray.add(1, "A");
```
232.clear()
```
import PlainArray from "@ohos.util.PlainArray"
let plainArray = new PlainArray();
plainArray.add(1, "A");
plainArray.add(2, "B");
plainArray.add(3, "C");
plainArray.clear();
```
233.clone()
```
import PlainArray from "@ohos.util.PlainArray"
let plainArray = new PlainArray();
plainArray.add(1, "A");
plainArray.add(2, "B");
plainArray.add(3, "C");
let clonePlainarray = plainArray.clone();
```
234.has()
```
import PlainArray from "@ohos.util.PlainArray"
let plainArray = new PlainArray();
plainArray.add(1, "A");
plainArray.add(2, "B");
plainArray.add(3, "C");
let res = plainArray.has(6);
```
235.get()
```
import PlainArray from "@ohos.util.PlainArray"
let plainArray = new PlainArray();
plainArray.add(1, "A");
plainArray.add(2, "B");
plainArray.add(3, "C");
let value = plainArray.get(2);
```
236.getIndexOfKey()
```
import PlainArray from "@ohos.util.PlainArray"
let plainArray = new PlainArray();
plainArray.add(1, "A");
plainArray.add(2, "B");
plainArray.add(3, "C");
let res = plainArray.getIndexOfKey(9);
```
237.getIndexOfValue()
```
import PlainArray from "@ohos.util.PlainArray"
let plainArray = new PlainArray();
plainArray.add(1, "A");
plainArray.add(2, "B");
plainArray.add(3, "C");
let res = plainArray.getIndexOfValue("Asad");
```
238.isEmpty()
```
import PlainArray from "@ohos.util.PlainArray"
let plainArray = new PlainArray();
plainArray.add(1, "A");
plainArray.add(2, "B");
plainArray.add(3, "C");
let res = plainArray.isEmpty();
```
239.getKeyAt()
```
import PlainArray from "@ohos.util.PlainArray"
let plainArray = new PlainArray();
plainArray.add(1, "A");
plainArray.add(2, "B");
plainArray.add(3, "C");
let res = plainArray.getKeyAt(2);
```
240.remove()
```
import PlainArray from "@ohos.util.PlainArray"
let plainArray = new PlainArray();
plainArray.add(1, "A");
plainArray.add(2, "B");
plainArray.add(3, "C");
let res = plainArray.remove(2);
```
241.removeAt()
```
import PlainArray from "@ohos.util.PlainArray"
let plainArray = new PlainArray();
plainArray.add(1, "A");
plainArray.add(2, "B");
plainArray.add(3, "C");
let res = plainArray.removeAt(2);
```
242.removeRangeFrom()
```
import PlainArray from "@ohos.util.PlainArray"
let plainArray = new PlainArray();
plainArray.add(1, "A");
plainArray.add(2, "B");
plainArray.add(3, "C");
plainArray.removeRangeFrom(2, 2);
```
243.setValueAt()
```
import PlainArray from "@ohos.util.PlainArray"
let plainArray = new PlainArray();
plainArray.add(1, "A");
plainArray.add(2, "B");
plainArray.add(3, "C");
plainArray.setValueAt(2, "V");
```
244.toString()
```
import PlainArray from "@ohos.util.PlainArray"
let plainArray = new PlainArray();
plainArray.add(1, "A");
plainArray.add(2, "B");
plainArray.add(3, "C");
let res1 = plainArray.toString();
```
245.getValueAt()
```
import PlainArray from "@ohos.util.PlainArray"
let plainArray = new PlainArray();
plainArray.add(1, "A");
plainArray.add(2, "B");
plainArray.add(3, "C");
let res = plainArray.getValueAt(2);
```
246.forEach()
```
import PlainArray from "@ohos.util.PlainArray"
let plainArray = new PlainArray();
plainArray.add(1, "A");
plainArray.add(2, "B");
plainArray.add(3, "C");
let arr = [];
res.forEach((value, index) => {
    arr.push(value);
});
```
247.[Symbol.iterator]()
```
import PlainArray from "@ohos.util.PlainArray"
let plainArray = new PlainArray();
plainArray.add(1, "A");
plainArray.add(2, "B");
plainArray.add(3, "C");
let iters = plainArray[Symbol.iterator]();
```
248.add()
```
import Queue from "@ohos.util.Queue"
let queue = new Queue();
queue.add("四");
```
249.getFirst()
```
import Queue from "@ohos.util.Queue"
let queue = new Queue();
queue.add("四");
let res = queue.getFirst();
```
250.pop()
```
import Queue from "@ohos.util.Queue"
let queue = new Queue();
queue.add("四");
let res = queue.pop();
```
251.forEach()
```
import Queue from "@ohos.util.Queue"
let queue = new Queue();
queue.add(1);
queue.add(2);
queue.add(3);
queue.add(4);
queue.forEach((item, index) => {
    arr.push(item);
});
```
252.[Symbol.iterator]()
```
import Queue from "@ohos.util.Queue"
let queue = new Queue();
queue.add("四");
let itr = queue[Symbol.iterator]();
```
253.isEmpty()
```
import Stack from "@ohos.util.Stack"
let stack = new Stack();
stack.push(8);
stack.push(5);
let res = stack.isEmpty();
```
254.peek()
```
import Stack from "@ohos.util.Stack"
let stack = new Stack();
stack.push(1234);
let res = stack.peek();
```
255.pop()
```
import Stack from "@ohos.util.Stack"
let stack = new Stack();
stack.push(1234);
let res = stack.pop();
```
256.push()
```
import Stack from "@ohos.util.Stack"
let stack = new Stack();
stack.push(12);
```
257.locate()
```
import Stack from "@ohos.util.Stack"
let stack = new Stack();
stack.push("A");
stack.push("B");
stack.push(1);
let res = stack.locate("A");
```
258.forEach()
```
import Stack from "@ohos.util.Stack"
let stack = new Stack();
stack.push(8);
stack.push(5);
stack.push(c);
stack.push(6);
stack.forEach((item, index) => {
    arr.push(item);
});
```
259.[Symbol.iterator]()
```
import Stack from "@ohos.util.Stack"
let stack = new Stack();
stack.push("A");
stack.push("B");
stack.push(1);
let itr = stack[Symbol.iterator]();
```
260.isEmpty()
```
import TreeMap from "@ohos.util.TreeMap"
let treeMap = new TreeMap();
treeMap.set(0, "a");
treeMap.set(1, "b");
let res = treeMap.isEmpty();
```
261.hasKey()
```
import TreeMap from "@ohos.util.TreeMap"
let treeMap = new TreeMap();
treeMap.set(0, "a");
treeMap.set(1, "b");
let res = treeMap.hasKey(1);
```
262.hasValue()
```
import TreeMap from "@ohos.util.TreeMap"
let treeMap = new TreeMap();
treeMap.set(0, "a");
treeMap.set(1, "b");
let res = treeMap.hasValue("a");
```
263.get()
```
import TreeMap from "@ohos.util.TreeMap"
let treeMap = new TreeMap();
treeMap.set(0, "a");
treeMap.set(1, "b");
let res = treeMap.get(1);
```
264.getFirstKey()
```
import TreeMap from "@ohos.util.TreeMap"
let treeMap = new TreeMap();
treeMap.set(0, "a");
treeMap.set(1, "b");
let res = treeMap.getFirstKey();
```
265.getLastKey()
```
import TreeMap from "@ohos.util.TreeMap"
let treeMap = new TreeMap();
treeMap.set(0, "a");
treeMap.set(1, "b");
let res = treeMap.getLastKey();
```
266.setAll()
```
import TreeMap from "@ohos.util.TreeMap"
let treeMap = new TreeMap();
treeMap.set(0, "a");
treeMap.set(1, "b");
let treeMap1 = new TreeMap();
treeMap1.set(0, "a");
treeMap1.set(1, "b");
treeMap1.set(2, "c");
treeMap.setAll(treeMap1);
```
267.set()
```
import TreeMap from "@ohos.util.TreeMap"
let treeMap = new TreeMap();
treeMap.set(0, "a");
treeMap.set(1, "b");
```
268.remove()
```
import TreeMap from "@ohos.util.TreeMap"
let treeMap = new TreeMap();
treeMap.set(0, "a");
treeMap.set(1, "b");
let res = treeMap.remove(1);
```
269.clear()
```
import TreeMap from "@ohos.util.TreeMap"
let treeMap = new TreeMap();
treeMap.set(0, "a");
treeMap.set(1, "b");
treeMap.clear();
```
270.getLowerKey()
```
import TreeMap from "@ohos.util.TreeMap"
let treeMap = new TreeMap();
treeMap.set(0, "a");
treeMap.set(1, "b");
let res = treeMap.getLowerKey(1);
```
271.getHigherKey()
```
import TreeMap from "@ohos.util.TreeMap"
let treeMap = new TreeMap();
treeMap.set(0, "a");
treeMap.set(1, "b");
let res = treeMap.getHigherKey(1);
```
272.keys()
```
import TreeMap from "@ohos.util.TreeMap"
let treeMap = new TreeMap();
treeMap.set(0, "a");
treeMap.set(1, "b");
let res = treeMap.keys();
```
273.values()
```
import TreeMap from "@ohos.util.TreeMap"
let treeMap = new TreeMap();
treeMap.set(0, "a");
treeMap.set(1, "b");
let res = treeMap.values();
```
274.replace()
```
import TreeMap from "@ohos.util.TreeMap"
let treeMap = new TreeMap();
treeMap.set(0, "a");
treeMap.set(1, "b");
let res = treeMap.replace(1, "B");
```
275.forEach()
```
import TreeMap from "@ohos.util.TreeMap"
let treeMap = new TreeMap();
treeMap.set(0, "a");
treeMap.set(1, "b");
treeMap.set(2, "c");
treeMap.set(3, "d");
treeMap.set(4, "g");
let arr1 = [];
treeMap.forEach((value, key) => {
    arr1.push(value);
});
```
276.entries()
```
import TreeMap from "@ohos.util.TreeMap"
let treeMap = new TreeMap();
treeMap.set(0, "a");
treeMap.set(1, "b");
treeMap.set(2, "c");
let entries = treeMap.entries();
```
277.[Symbol.iterator]()
```
import TreeMap from "@ohos.util.TreeMap"
let treeMap = new TreeMap();
treeMap.set(0, "a");
treeMap.set(1, "b");
treeMap.set(2, "c");
let iters = treeMap[Symbol.iterator]();
```
278.isEmpty()
```
import TreeSet from "@ohos.util.TreeSet"
let treeSet = new TreeSet();
treeSet.add("a");
let res = treeSet.isEmpty();
```
279.has()
```
import TreeSet from "@ohos.util.TreeSet"
let treeSet = new TreeSet();
treeSet.add("a");
treeSet.add("b");
treeSet.add("c");
let res = treeSet.has("c");
```
280.add()
```
import TreeSet from "@ohos.util.TreeSet"
let treeSet = new TreeSet();
treeSet.add("a");
treeSet.add("b");
treeSet.add("c");
```
281.remove()
```
import TreeSet from "@ohos.util.TreeSet"
let treeSet = new TreeSet();
treeSet.add("a");
treeSet.add("b");
treeSet.add("c");
let res = treeSet.remove("c");
```
282.clear()
```
import TreeSet from "@ohos.util.TreeSet"
let treeSet = new TreeSet();
treeSet.add("a");
treeSet.add("b");
treeSet.add("c");
treeSet.clear();
```
283.getFirstValue()
```
import TreeSet from "@ohos.util.TreeSet"
let treeSet = new TreeSet();
treeSet.add("a");
treeSet.add("b");
treeSet.add("c");
let res = treeSet.getFirstValue();
```
284.getLastValue()
```
import TreeSet from "@ohos.util.TreeSet"
let treeSet = new TreeSet();
treeSet.add("a");
treeSet.add("b");
treeSet.add("c");
let res = treeSet.getLastValue();
```
285.getLowerValue()
```
import TreeSet from "@ohos.util.TreeSet"
let treeSet = new TreeSet();
treeSet.add(0);
treeSet.add(1);
treeSet.add(2);
treeSet.add(3);
let res = treeSet.getLowerValue(2);
```
286.getHigherValue()
```
import TreeSet from "@ohos.util.TreeSet"
let treeSet = new TreeSet();
treeSet.add(0);
treeSet.add(1);
treeSet.add(2);
treeSet.add(3);
let res = treeSet.getHigherValue(3);
```
287.popFirst()
```
import TreeSet from "@ohos.util.TreeSet"
let treeSet = new TreeSet();
treeSet.add(0);
treeSet.add(1);
treeSet.add(2);
treeSet.add(3);
let res = treeSet.popFirst();
```
288.popLast()
```
import TreeSet from "@ohos.util.TreeSet"
let treeSet = new TreeSet();
treeSet.add(0);
treeSet.add(1);
treeSet.add(2);
treeSet.add(3);
let res = treeSet.popLast();
```
289.forEach()
```
import TreeSet from "@ohos.util.TreeSet"
let treeSet = new TreeSet();
treeSet.add(0);
treeSet.add(1);
treeSet.add(2);
treeSet.add(3);
treeSet.add(4);
let arr1 = [];
treeSet.forEach((value, key) => {
    arr1.push(value);
});
```
290.values()
```
import TreeSet from "@ohos.util.TreeSet"
let treeSet = new TreeSet();
treeSet.add("a");
treeSet.add("b");
treeSet.add("c");
let res = treeSet.values();
```
291.entries()
```
import TreeSet from "@ohos.util.TreeSet"
let treeSet = new TreeSet();
treeSet.add("a");
treeSet.add("b");
treeSet.add("c");
let res = treeSet.entries();
```
292.[Symbol.iterator]()
```
import TreeSet from "@ohos.util.TreeSet"
let treeSet = new TreeSet();
treeSet.add("a");
treeSet.add("b");
treeSet.add("c");
let res = treeSet[Symbol.iterator]();
```

## 3. js_sys_module子模块
### 3.1. 简介
进程主要用于获取进程的相关ID，获取和修改进程的工作目录，退出和关闭进程。 childprocess 对象可用于创建新进程。 主进程可以获取子进程的标准输入输出，发送信号，关闭子进程。
### 3.2. 目录

```
commomlibrary/ets_utils/js_sys_module/
├── Class:PROCESS                   # PROCESS类
├── Uid                             # Uid属性
├── Gid                             # Gid属性
├── EUid                            # EUid属性
├── EGid                            # EGid属性
├── Groups                          # Groups属性
├── Pid                             # Pid属性
├── Ppid                            # Ppid属性
├── chdir()                         # chdir方法
├── uptime()                        # uptime方法
├── kill()                          # kill方法
├── abort()                         # abort方法
├── on()                            # on方法
├── tid                             # tid方法
├── getStartRealtime()              # getStartRealtime方法
├── getAvailableCores()             # getAvailableCores方法
├── getPastCputime()                # getPastCputime方法
├── isIsolatedProcess()             # isIsolatedProcess方法
├── is64Bit()                       # is64Bit方法
├── isAppUid()                      # isAppUid方法
├── getUidForName()                 # getUidForName方法
├── getThreadPriority()             # getThreadPriority方法
├── getSystemConfig()               # getSystemConfig方法
├── getEnvironmentVar()             # getEnvironmentVar方法
├── exit()                          # exit方法
├── cwd()                           # cwd方法
├── off()                           # off方法
├── runCmd()                        # runCmd方法
└─── Class:CHILDPROCESS             # class of CHILDPROCESS类
    ├── close()                     # close方法
    ├── kill()                      # kill方法
    ├── getOutput()                 # getOutput方法
    ├── getErrorOutput()            # getErrorOutput方法
    ├── wait()                      # wait方法
    ├── killed                      # killed属性
    ├── pid                         # pid属性
    ├── ppid                        # ppid属性
    └── exitCode                    # exitCode属性
```

### 3.3. 说明

#### 3.3.1. 接口说明
| 接口名 | 说明 |
| -------- | -------- |
| const uid :number | 返回进程的数字用户 ID。 |
| const gid :number | 返回进程的数字组 ID。 |
| const euid :number | 返回进程的数字有效用户身份。 |
| const egid :number | 返回 node.js 进程的数字有效组 ID。 |
| const groups :number[] |  返回具有补充组 ID 的数组。 |
| const pid :number | 返回进程的PID。 |
| const ppid :number |  返回当前进程的父进程的PID。 |
| chdir(dir:string) :void | 更改 node.js 进程的当前工作目录。 |
| uptime() :number |  返回当前系统已经运行的秒数。 |
| Kill(pid:number, signal:number) :boolean | 将信号发送到识别的进程PID，true表示发送成功。 |
| abort() :void | 导致 node.js 进程立即退出并生成核心文件。 |
| on(type:string ,listener:EventListener) :void | 用于存储用户触发的事件。 |
| exit(code:number):void | 导致 node.js 进程立即退出。 |
| cwd():string |  返回 node.js 进程的当前工作目录。 |
| off(type: string): boolean | 清除用户存储的事件。 True 表示清算成功。 |
| runCmd(command: string, options?: { timeout : number, killSignal : number \| string, maxBuffer : number }): ChildProcess |通过runcmd，你可以fork一个新进程来运行一个shell并返回childprocess对象。 第一个参数command指的是要运行的shell，第二个参数options指的是子进程的一些运行参数。 这些参数主要是指 timeout、killsignal 和 maxbuffer。 如果设置了timeout，则子进程会在超时后发送killsignal信号。 Maxbuffer 用于限制可以接收的最大 stdout 和 stderr 大小。 |
| wait()： Promise\<number> | 用于等待子进程运行并返回promise对象，其值为子进程的退出码。 |
| getOutput(): Promise\<Uint8Array> |  用于获取子进程的标准输出。 |
| getErrorOutput(): Promise\<Uint8Array> | 用于获取子进程的标准错误输出。 |
| const tid:number | 返回进程的 TID。 |
| getStartRealtime() :number | 获取从系统启动到进程启动所经过的实时时间（以毫秒为单位）。 |
| getAvailableCores() :number[] | 获取多核设备上当前进程可用的 CPU 内核。 |
| getPastCputime() :number | 获取从进程开始到当前时间的 CPU 时间（以毫秒为单位）。 |
| isIsolatedProcess(): boolean | 检查进程是否被隔离。 |
| is64Bit(): boolean | 检查进程是否在 64 位环境中运行。 |
| isAppUid(v:number): boolean | 检查指定的 uid 是否属于特定应用程序。 |
| getUidForName(v:string): number | 根据用户名获取用户所属的用户组ID |
| getThreadPriority(v:number): number | 根据指定的 TID 获取线程优先级。 |
| getSystemConfig(name:number): number | 根据指定的系统配置名称获取系统的配置。 |
| getEnvironmentVar(name:string): string | 根据环境变量的名称获取对应的值。 |
| close(): void | 用于关闭正在运行的子进程。 |
| kill(signal: number \| string): void |  用于向子进程发送信号。 |
| readonly killed: boolean | 表示信号是否发送成功，true表示信号发送成功。 |
| readonly exitCode: number | 表示子进程的退出代码。 |
| readonly pid: number | 表示子进程ID。 |
| readonly ppid: number | 代表主进程ID。 |

#### 3.3.2. 使用说明

各接口使用方法如下：
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

## 4. js_concurrent_module子模块

### 4.1. 简介

worker能够让js拥有多线程的能力，通过postMessage完成worker线程与宿主线程通信。

### 4.2. 接口说明
接口实现详见：js_concurrent_module/worker

#### 4.2.1. Worker对象描述

宿主线程用于与worker线程通信的Object对象。

##### 4.2.1.1. 接口

1. 

- 接口名

|constructor(scriptURL:string, options?:WorkerOptions) | 构造函数 |
|---|---|

- 使用示例

通过判断存放worker.ts的workers目录是否与pages目录同级（以下简称目录同级）和创建项目的模型类别，共有以下四种新建Worker的方式：

(1) FA模型: 目录同级
```
import worker from "@ohos.worker";
const workerInstance = new worker.Worker("workers/worker.js", {name:"first worker"});
```
(2) FA模型: 目录不同级（以workers目录放置pages目录前一级为例）
```
import worker from "@ohos.worker";
const workerInstance = new worker.Worker("../workers/worker.js", {name:"first worker"});
```
(3)  Stage模型: 目录同级
```
import worker from "@ohos.worker";
const workerInstance = new worker.Worker('entry/ets/workers/worker.ts');
```
(4) Stage模型: 目录不同级（以workers目录放置pages目录后一级为例）
```
import worker from "@ohos.worker";
const workerInstance = new worker.Worker('entry/ets/pages/workers/worker.ts');
```
对于Stage模型中scriptURL——"entry/ets/workers/worker.ts"的解释：
- entry: 为module.json5中module中name属性的值；
- ets: 表明当前工程使用的语言；
- worker.ts: 创建worker.ts文件或者worker.js文件都可以。

另外，需在工程的模块级build-profile.json5文件的buildOption属性中添加配置信息，主要分为下面两种情况：

(1) 目录同级(**此情况不添加亦可**)

FA模型:
```
  "buildOption": {
    "sourceOption": {
      "workers": [
        "./src/main/ets/MainAbility/workers/worker.js"
      ]
    }
  }
```
Stage模型:
```
  "buildOption": {
    "sourceOption": {
      "workers": [
        "./src/main/ets/workers/worker.ts"
      ]
    }
  }
```
(2) 目录不同级(**此情况必须添加**)

FA模型:
```
  "buildOption": {
    "sourceOption": {
      "workers": [
        "./src/main/ets/workers/worker.js"
      ]
    }
  }
```
Stage模型(workers目录放在pages目录里为例):
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

- 接口名

| postMessage(message:Object, options?:PostMessageOptions)  | 向worker线程发送消息  |
|---|---|
| postMessage(message:Object, transfer:ArrayBuffer[])  | 向worker线程发送消息  |

- 使用示例

```
// 示例一
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
worker.postMessage("hello world");
 
// 示例二
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
var buffer = new ArrayBuffer(8);
worker.postMessage(buffer, [buffer]);
```

3. 

- 接口名

| on(type:string, listener:EventListener)  | 向worker添加一个事件监听  |
|---|---|

- 使用示例

```
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
worker.on("alert", (e)=>{
     console.log("worker on...");
});
```

4. 

- 接口名

| once(type:string, listener:EventListener)  | 向worker添加一个事件监听, 事件监听只执行一次便自动删除  |
|---|---|

- 使用示例

```
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
worker.once("alert", (e)=>{
    console.log("worker once...");
});
```

5. 

- 接口名

| off(type:string, listener?:EventListener)  | 删除worker的事件监听  |
|---|---|

- 使用示例

```
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
worker.off("alert");
```

6. 

- 接口名

| terminate()  | 关闭worker线程，终止worker发送消息  |
|---|---|

- 使用示例

```
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
worker.terminate();
```

7. 

- 接口名

| removeEventListener(type:string, listener?:EventListener)  | 删除worker的事件监听  |
|---|---|

- 使用示例

```
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
worker.removeEventListener("alert", (e)=>{
    console.log("worker removeEventListener...");
});
```

8. 

- 接口名

| dispatchEvent(event: Event)  | 分发定义在worker的事件  |
|---|---|

- 使用示例

```
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
worker.dispatchEvent({type:"alert"});
```

9. 

- 接口名

| removeAllListener()  | 删除worker的所有事件监听  |
|---|---|

- 使用示例

```
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
worker.removeAllListener();
```

##### 4.2.1.2. 属性

1. 

- 属性名

| onexit?:(code:number)=>void  | worker退出时被调用的事件处理程序，处理程序在宿主线程中执行  |
|---|---|

- 使用示例

```
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
worker.onexit = function(e) {
    console.log("onexit...");
}
```

2. 

- 属性名

| onerror?:(ev:ErrorEvent)=>void  | worker在执行过程中发生异常被调用的事件处理程序，处理程序在宿主线程中执行  |
|---|---|

- 使用示例

```
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
worker.onerror = function(e) {
    console.log("onerror...");
}
```

3. 

- 属性名

| onmessage?:(ev:MessageEvent)=>void  | 宿主线程收到来自其创建的worker通过parentPort.postMessage接口发送的消息时被调用的事件处理程序， 处理程序在宿主线程中执行 |
|---|---|

- 使用示例

```
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
worker.onmessage = function(e) {
    console.log("onmessage...");
}
```

4. 

- 属性名

| onmessageerror?:(event:MessageEvent)=>void  | worker对象接收到一条无法序列化的消息时被调用的事件处理程序， 处理程序在宿主线程中执行 |
|---|---|

- 使用示例

```
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
worker.onmessageerror = function(e) {
    console.log("onmessageerror...");
}
```

#### 4.2.2. parentPort对象描述

worker线程用于与宿主线程通信的Object对象。

##### 4.2.2.1. 接口

1. 

- 接口名

| postMessage(message:Object, options?:PostMessageOptions)  | 向宿主线程发送消息 |
|---|---|
| postMessage(message:Object, transfer:ArrayBuffer[])  | 向宿主线程发送消息  |

- 使用示例

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

- 接口名

| close()  | 关闭worker线程，终止worker接收消息  |
|---|---|

- 使用示例

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

##### 4.2.2.2. 属性

1. 

- 属性名

| onmessage?:(event:MessageEvent)=>void  | 宿主线程收到来自其创建的worker通过worker.postMessage接口发送的消息时被调用的事件处理程序，处理程序在worker线程中执行  |
|---|---|

- 使用示例

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

- 属性名

| onerror?:(ev: ErrorEvent)=>void  | worker在执行过程中发生异常被调用的事件处理程序，处理程序在worker线程中执行  |
|---|---|

- 使用示例

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

- 属性名

| onmessageerror?:(event: MessageEvent)=>void  | worker对象接收到一条无法被反序列化的消息时被调用的事件处理程序， 处理程序在worker线程中执行  |
|---|---|

- 使用示例

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

### 4.3. 涉及仓

[arkcompiler_ets_runtime](https://gitee.com/openharmony/arkcompiler_ets_runtime/blob/master/README_zh.md)
[arkui_ace_engine](https://gitee.com/openharmony/arkui_ace_engine/blob/master/README_zh.md)
[arkui_napi](https://gitee.com/openharmony/arkui_napi/blob/master/README_zh.md)


# 相关仓

[ets_utils子系统](https://gitee.com/openharmony/commonlibrary_ets_utils/blob/master/README.md)

# 许可证

js_api_module子模块在[Mozilla许可证](https://www.mozilla.org/en-US/MPL/)下可用，有关完整的许可证文本，请参见[许可证](https://gitee.com/openharmony/commonlibrary_ets_utils/blob/master/js_api_module/mozilla_docs.txt)。

js_util_module子模块在[Mozilla许可证](https://www.mozilla.org/en-US/MPL/)下可用，有关完整的许可证文本，请参见[许可证](https://gitee.com/openharmony/commonlibrary_ets_utils/blob/master/js_util_module/mozilla_docs.txt)。

js_concurrent_module子模块在[Mozilla许可证](https://www.mozilla.org/en-US/MPL/)下可用，有关完整的许可证文本，请参见[许可证](https://gitee.com/openharmony/commonlibrary_ets_utils/blob/master/js_concurrent_module/mozilla_docs.txt)。

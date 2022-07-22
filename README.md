# ets_utils组件
-   [子模块](#子模块)
    -   [1.js_api_module子模块](#1.js_api_module子模块)
        -   [1.1.简介](#1.1.简介)
        -   [1.2.目录](#1.2.目录)
        -   [1.3.说明](#1.3.说明)
            -   [1.3.1.接口说明](#1.3.1.接口说明)
            -   [1.3.2.使用说明](#1.3.2.使用说明)
    -   [2.js_util_module子模块](#2.js_util_module子模块)
        -   [2.1.简介](#2.1.简介)
        -   [2.2.目录](#2.2.目录)
        -   [2.3.说明](#2.3.说明)
            -   [2.3.1.接口说明](#2.3.1.接口说明)
            -   [2.3.2.使用说明](#2.3.2.使用说明)
    -   [3.js_sys_module子模块](#3.js_sys_module子模块)
        -   [3.1.简介](#3.1.简介)
        -   [3.2.目录](#3.2.目录)
        -   [3.3.说明](#3.3.说明)
            -   [3.3.1.接口说明](#3.3.1.接口说明)
            -   [3.3.2.使用说明](#3.3.2.使用说明)
    -   [4.js_worker_module子模块](#4.js_worker_module子模块)
        -   [4.1.简介](#4.1.简介)
        -   [4.2.接口说明](#4.2.接口说明)
            -   [4.2.1.Worker对象描述](#4.2.1.Worker对象描述)
                -   [4.2.1.1.接口](#4.2.1.1.接口)
                -   [4.2.1.2.属性](#4.2.1.2.属性)
            -   [4.2.2.Worker对象描述](#4.2.2.Worker对象描述)
                -   [4.2.2.1.接口](#4.2.2.1.接口)
                -   [4.2.2.1.属性](#4.2.2.1.属性)
        -   [4.3.涉及仓](#4.3.涉及仓)

# 子模块
## 1.js_api_module子模块
### 1.1.简介

URL接口用于解析，构造，规范化和编码 URLs。 URL的构造函数创建新的URL对象。 以便对URL的已解析组成部分或对URL进行更改。URLSearchParams 接口定义了一些实用的方法来处理 URL 的查询字符串。

URI表示统一资源标识符引用。

xml表示指可扩展标记语言。

XmlSerializer接口用于生成一个xml文件。 XmlSerializer的构造函数创建新的XmlSerializer对象，调用XmlSerializer对象的方法生成一个xml文件。XmlPullParser 接口用于解析已有的xml文件，XmlPullParser的构造函数创建新的XmlPullParser对象，调用XmlPullParser对象的方法解析xml。

### 1.2.目录

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

### 1.3.说明

#### 1.3.1.接口说明


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
#### 1.3.2.使用说明

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

## 2.js_util_module子模块
### 2.1.简介

UTIL接口用于字符编码TextEncoder、解码TextDecoder、帮助函数HelpFunction、基于Base64的字节编码encode和解码decode、有理数RationalNumber。TextEncoder表示一个文本编码器，接受字符串作为输入，以UTF-8格式进行编码，输出UTF-8字节流。TextDecoder接口表示一个文本解码器，解码器将字节流作为输入，输出stirng字符串。HelpFunction主要是对函数做callback化、promise化以及对错误码进行编写输出，及类字符串的格式化输出。encode接口使用Base64编码方案将指定u8数组中的所有字节编码到新分配的u8数组中或者使用Base64编码方案将指定的字节数组编码为String。decode接口使用Base64编码方案将Base64编码的字符串或输入u8数组解码为新分配的u8数组。RationalNumber有理数主要是对有理数进行比较，获取分子分母等方法。LruBuffer该算法在缓存空间不够的时候，将近期最少使用的数据替换为新数据。该算法源自这样一种访问资源的需求：近期访问的数据，可能在不久的将来会再次访问。于是最少访问的数据就是价值最小的，是最应该踢出缓存空间的数据。Scope接口用于描述一个字段的有效范围。 Scope实例的构造函数用于创建具有指定下限和上限的对象，并要求这些对象必须具有可比性。
### 2.2.目录

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
└── Class:Types                         # Types类
    ├── isAnyArrayBuffer()              # isAnyArrayBuffer方法
    ├── isArrayBufferView()             # isArrayBufferView方法
    ├── isArgumentsObject()             # isArgumentsObject方法
    ├── isArrayBuffer()                 # isArrayBuffer方法
    ├── isAsyncFunction()               # isAsyncFunction方法
    ├── isBigInt64Array()               # isBigInt64Array方法
    ├── isBigUint64Array()              # isBigUint64Array方法
    ├── isBooleanObject()               # isBooleanObject方法
    ├── isBoxedPrimitive()              # isBoxedPrimitive方法
    ├── isDataView()                    # isDataView方法
    ├── isDate()                        # isDate方法
    ├── isExternal()                    # isExternal方法
    ├── isFloat32Array()                # isFloat32Array方法
    ├── isFloat64Array()                # isFloat64Array方法
    ├── isGeneratorFunction()           # isGeneratorFunction方法
    ├── isGeneratorObject()             # isGeneratorObject方法
    ├── isInt8Array()                   # isInt8Array方法
    ├── isInt16Array()                  # isInt16Array方法
    ├── isInt32Array()                  # isInt32Array方法
    ├── isMap()                         # isMap方法
    ├── isMapIterator()                 # isMapIterator方法
    ├── isModuleNamespaceObject()       # isModuleNamespaceObject方法
    ├── isNativeError()                 # isNativeError方法
    ├── isNumberObject()                # isNumberObject方法
    ├── isPromise()                     # isPromise方法
    ├── isProxy()                       # isProxy方法
    ├── isRegExp()                      # isRegExp方法
    ├── isSet()                         # isSet方法
    ├── isSetIterator()                 # isSetIterator方法
    ├── isSharedArrayBuffer()           # isSharedArrayBuffer方法
    ├── isStringObject()                # isStringObject方法
    ├── isSymbolObject()                # isSymbolObject方法
    ├── isTypedArray()                  # isTypedArray方法
    ├── isUint8Array()                  # isUint8Array方法
    ├── isUint8ClampedArray()           # isUint8ClampedArray方法
    ├── isUint16Array()                 # isUint16Array方法
    ├── isUint32Array()                 # isUint32Array方法
    ├── isWeakMap()                     # isWeakMap方法
    └── isWeakSet()                     # isWeakSet方法
```
### 2.3.说明

#### 2.3.1.接口说明


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

#### 2.3.2.使用说明
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
9.printf()
```
import util from '@ohos.util'
var format = "%%%o%%%i%s";
var value =  function aa(){};
var value1 = 1.5;
var value2 = "qwer";
var result = util.printf(format,value,value1,value2);
```
10.getErrorString()
```
import util from '@ohos.util'
var errnum = 13;
var result = util.getErrorString(errnum);
```
11.callbackWrapper()
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
12.promiseWrapper()
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
13.encodeSync()
```
import util from '@ohos.util'
var that = new util.Base64();
var array = new Uint8Array([115,49,51]);
var result = that.encodeSync(array);
```
14.encodeToStringSync()
```
import util from '@ohos.util'
var that = new util.Base64();
var array = new Uint8Array([115,49,51]);
var result = that.encodeToStringSync(array);
```
15.decodeSync()
```
import util from '@ohos.util'
var that = new util.Base64()
var buff = 'czEz';
var result = that.decodeSync(buff);

```
16.encode()
```
import util from '@ohos.util'
var that = new util.Base64()
var array = new Uint8Array([115,49,51]);
await that.encode(array).then(val=>{
})
done()
```
17.encodeToString()
```
import util from '@ohos.util'
var that = new util.Base64()
var array = new Uint8Array([115,49,51]);
await that.encodeToString(array).then(val=>{
})
done()
```
18.decode()
```
import util from '@ohos.util'
var that = new util.Base64()
var buff = 'czEz';
await that.decode(buff).then(val=>{
})
done()
```
19.createRationalFromString()
```
import util from '@ohos.util'
var pro = new util.RationalNumber(0, 0);
var res = pro.createRationalFromString("-1:2");
var result1 = res.valueOf();
```
20.compareTo()
```
import util from '@ohos.util'
var pro = new util.RationalNumber(2, 1);
var proc = new util.RationalNumber(3, 4);
var res = pro.compareTo(proc);
```
21.equals()
```
import util from '@ohos.util'
var pro = new util.RationalNumber(2, 1);
var proc = new util.RationalNumber(3, 4);
var res = pro.equals(proc);
```
22.valueOf()
```
import util from '@ohos.util'
var pro = new util.RationalNumber(2, 1);
var res = pro.valueOf();
```
23.getCommonDivisor()
```
import util from '@ohos.util'
var pro = new util.RationalNumber(0, 0);
var res = pro.getCommonDivisor(4, 8);
```
24.getDenominator()
```
import util from '@ohos.util'
var pro = new util.RationalNumber(2, 1);
var res = pro.getDenominator();
```
25.getNumerator()
```
import util from '@ohos.util'
var pro = new util.RationalNumber(-2, 1);
var res = pro.getNumerator();
```
26.isFinite()
```
import util from '@ohos.util'
var pro = new util.RationalNumber(-2, 1);
var res = pro.isFinite();
```
27.isNaN()
```
import util from '@ohos.util'
var pro = new util.RationalNumber(-2, 1);
var res = pro.isNaN();
```
28.isZero()
```
import util from '@ohos.util'
var pro = new util.RationalNumber(-2, 1);
var res = pro.isZero();
```
29.toString()
```
import util from '@ohos.util'
var pro = new util.RationalNumber(-2, 1);
var res = pro.toString();
```
30.updateCapacity()
```
import util from '@ohos.util'
var pro = new util.LruBuffer();
var result = pro.updateCapacity(100);
```
31.toString()
```
import util from '@ohos.util'
var pro = new util.LruBuffer();
pro.put(2,10);
pro.get(2);
pro.remove(20);
var result = pro.toString();
```
32.values()
```
import util from '@ohos.util'
var pro = new util.LruBuffer();
pro.put(2,10);
pro.put(2,"anhu");
pro.put("afaf","grfb");
var result = pro.values();
```
33.length
```
import util from '@ohos.util'
var pro = new util.LruBuffer();
pro.put(2,10);
pro.put(1,8);
var result = pro.length;
```
34.getCapacity()
```
import util from '@ohos.util'
var pro = new util.LruBuffer();
var result = pro.getCapacity();
```
35.clear()
```
import util from '@ohos.util'
var pro = new util.LruBuffer();
pro.put(2,10);
pro.clear();
```
36.getCreateCount()
```
import util from '@ohos.util'
var pro = new util.LruBuffer();
pro.put(1,8);
var result = pro.getCreateCount();
```
37.getMissCount()
```
import util from '@ohos.util'
var pro = new util.LruBuffer();
pro.put(2,10);
pro.get(2)
var result = pro.getMissCount();
```
38.getRemovalCount()
```

import util from '@ohos.util'
var pro = new util.LruBuffer();
pro.put(2,10);
pro.updateCapacity(2);
pro.put(50,22);
var result = pro.getRemovalCount();

```
39.getMatchCount()
```
import util from '@ohos.util'
var pro = new util.LruBuffer();
pro.put(2,10);
pro.get(2);
var result = pro.getMatchCount();
```
40.getPutCount()
```
import util from '@ohos.util'
var pro = new util.LruBuffer();
pro.put(2,10);
var result = pro.getPutCount();
```
41.isEmpty()
```
import util from '@ohos.util'
var pro = new util.LruBuffer();
pro.put(2,10);
var result = pro.isEmpty();
```
42.get()

```
import util from '@ohos.util'
var pro = new util.LruBuffer();
pro.put(2,10);
var result = pro.get(2);
```
43.put()
```
import util from '@ohos.util'
var pro = new util.LruBuffer();
var result = pro.put(2,10);
```
44.keys()
```
import util from '@ohos.util'
var pro = new util.LruBuffer();
pro.put(2,10);
var result = pro.keys();
```
45.remove()
```
import util from '@ohos.util'
var pro = new util.LruBuffer();
pro.put(2,10);
var result = pro.remove(20);
```
46.contains()
```
import util from '@ohos.util'
var pro = new util.LruBuffer();
pro.put(2,10);
var result = pro.contains(20);
```
47.createDefault()
```
import util from '@ohos.util'
var pro = new util.LruBuffer();
var result = pro.createDefault(50);
```
48.entries()
```
import util from '@ohos.util'
var pro = new util.LruBuffer();
pro.put(2,10);
var result = pro.entries();
```
49.\[Symbol.iterator\]()
```
import util from '@ohos.util'
var pro = new util.LruBuffer();
pro .put(2,10);
var result = pro[symbol.iterator]();
```
50.afterRemoval()
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

51.constructor()

```
var tempLower = new Temperature(30);
var tempUpper = new Temperature(40);
var range = new Scope(tempLower, tempUpper);
```

52.toString()

```
var tempLower = new Temperature(30);
var tempUpper = new Temperature(40);
var range = new Scope(tempLower, tempUpper);
var result = range.toString() // => [30,40]
```

53.intersect()

```
var tempLower = new Temperature(30);
var tempUpper = new Temperature(40);
var range = new Scope(tempLower, tempUpper);
var tempMiDF = new Temperature(35);
var tempMidS = new Temperature(39);
var rangeFir = new Scope(tempMiDF, tempMidS);
var result = range.intersect(rangeFir)  // => [35,39]
```

54.intersect()

```
var tempLower = new Temperature(30);
var tempUpper = new Temperature(40);
var tempMiDF = new Temperature(35);
var tempMidS = new Temperature(39);
var range = new Scope(tempLower, tempUpper);
var result = range.intersect(tempMiDF, tempMidS)  // => [35,39]
```

55.getUpper()

```
var tempLower = new Temperature(30);
var tempUpper = new Temperature(40);
var range = new Scope(tempLower, tempUpper);
var result = range.getUpper() // => 40
```

56.getLower()

```
var tempLower = new Temperature(30);
var tempUpper = new Temperature(40);
var range = new Scope(tempLower, tempUpper);
var result = range.getLower() // => 30
```

57.expand()

```
var tempLower = new Temperature(30);
var tempUpper = new Temperature(40);
var tempMiDF = new Temperature(35);
var tempMidS = new Temperature(39);
var range = new Scope(tempLower, tempUpper);
var result = range.expand(tempMiDF, tempMidS)  // => [30,40]
```

58.expand()

```
var tempLower = new Temperature(30);
var tempUpper = new Temperature(40);
var tempMiDF = new Temperature(35);
var tempMidS = new Temperature(39);
var range = new Scope(tempLower, tempUpper);
var rangeFir = new Scope(tempMiDF, tempMidS);
var result = range.expand(rangeFir) // => [30,40]
```

59.expand()

```
var tempLower = new Temperature(30);
var tempUpper = new Temperature(40);
var tempMiDF = new Temperature(35);
var range = new Scope(tempLower, tempUpper);
var result = range.expand(tempMiDF)  // => [30,40]
```

60.contains()

```
var tempLower = new Temperature(30);
var tempUpper = new Temperature(40);
var tempMiDF = new Temperature(35);
var range = new Scope(tempLower, tempUpper);
var result = range.contains(tempMiDF) // => true
```

61.contains()

```
var tempLower = new Temperature(30);
var tempUpper = new Temperature(40);
var range = new Scope(tempLower, tempUpper);
var tempLess = new Temperature(20);
var tempMore = new Temperature(45);
var rangeSec = new Scope(tempLess, tempMore);
var result = range.contains(rangeSec) // => true
```

62.clamp()

```
var tempLower = new Temperature(30);
var tempUpper = new Temperature(40);
var tempMiDF = new Temperature(35);
var range = new Scope(tempLower, tempUpper);
var result = range.clamp(tempMiDF) // => 35
```
63.isAnyArrayBuffer()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isAnyArrayBuffer(new ArrayBuffer([]))
```
64.isArrayBufferView()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isArrayBufferView(new DataView(new ArrayBuffer(16)));
```
65.isArgumentsObject()
```
import util from '@ohos.util'
function foo() {
        var result = proc.isArgumentsObject(arguments);
    }
var f = foo();
```
66.isArrayBuffer()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isArrayBuffer(new ArrayBuffer([]));
```
67.isAsyncFunction()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isAsyncFunction(async function foo() {});
```
68.isBigInt64Array()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isBigInt64Array(new Int16Array([]));
```
69.isBigUint64Array()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isBigUint64Array(new Int16Array([]));
```
70.isBooleanObject()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isBooleanObject(new Boolean(false));
```
71.isBoxedPrimitive()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isBoxedPrimitive(new Boolean(false));
```
72.isDataView()
```
import util from '@ohos.util'
var proc = new util.Types();
const ab = new ArrayBuffer(20);
var result = proc.isDataView(new DataView(ab));
```
73.isDate()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isDate(new Date());
```
74.isExternal()
```
import util from '@ohos.util'
const data = util.createExternalType();
var reult13 = proc.isExternal(data);
```
75.isFloat32Array()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isFloat32Array(new Float32Array([]));
```
76.isFloat64Array()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isFloat64Array(new Float64Array([]));
```
77.isGeneratorFunction()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isGeneratorFunction(function* foo() {});
```
78.isGeneratorObject()
```
import util from '@ohos.util'
var proc = new util.Types();
function* foo() {}
const generator = foo();
var result = proc.isGeneratorObject(generator);
```
79.isInt8Array()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isInt8Array(new Int8Array([]));
```
80.isInt16Array()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isInt16Array(new Int16Array([]));
```
81.isInt32Array()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isInt32Array(new Int32Array([]));
```
82.isMap()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isMap(new Map());
```
83.isMapIterator()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isMapIterator(map.keys());
```
84.isModuleNamespaceObject()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isModuleNamespaceObject(util);
```
85.isNativeError()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isNativeError(new TypeError());
```
86.isNumberObject()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isNumberObject(new Number(0));
```
87.isPromise()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isPromise(Promise.resolve(42));
```
88.isProxy()
```
import util from '@ohos.util'
var proc = new util.Types();
const target = {};
const proxy = new Proxy(target, {});
var result = proc.isProxy(proxy);
```
89.isRegExp()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isRegExp(new RegExp('abc'));
```
90.isSet()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isSet(new Set());
```
91.isSetIterator()
```
import util from '@ohos.util'
var proc = new util.Types();
const set = new Set();
var result = proc.isSetIterator(set.keys());
```
92.isSharedArrayBuffer()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isSharedArrayBuffer(new ArrayBuffer([]));
```
93.isStringObject()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isStringObject(new String('foo'));
```
94.isSymbolObject()
```
import util from '@ohos.util'
var proc = new util.Types();
const symbols = Symbol('foo');
var result = proc.isSymbolObject(Object(symbols));
```
95.isTypedArray()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isTypedArray(new Float64Array([]));
```
96.isUint8Array()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isUint8Array(new Uint8Array([]));
```
97.isUint8ClampedArray()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isUint8ClampedArray(new Uint8ClampedArray([]));
```
98.isUint16Array()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isUint16Array(new Uint16Array([]));
```
99.isUint32Array()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isUint32Array(new Uint32Array([]));
```
100.isWeakMap()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isWeakMap(new WeakMap());
```
101.isWeakSet()
```
import util from '@ohos.util'
var proc = new util.Types();
var result = proc.isWeakSet(new WeakSet());
```

## 3.js_sys_module子模块
### 3.1.简介
进程主要用于获取进程的相关ID，获取和修改进程的工作目录，退出和关闭进程。 childprocess 对象可用于创建新进程。 主进程可以获取子进程的标准输入输出，发送信号，关闭子进程。
### 3.2.目录

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

### 3.3.说明

#### 3.3.1.接口说明
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

#### 3.3.2.使用说明

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

## 4.js_worker_module子模块

### 4.1.简介

worker能够让js拥有多线程的能力，通过postMessage完成worker线程与宿主线程通信。

### 4.2.接口说明
接口实现详见：js_worker_module/jsapi/worker

#### 4.2.1.Worker对象描述

宿主线程用于与worker线程通信的Object对象。

##### 4.2.1.1接口

1. 

- 接口名

|constructor(scriptURL:string, options? WorkerOptions) | 构造函数 |
|---|---|

- 使用示例

```
import worker from "@ohos.worker"
const worker = new worker.Worker("workers/worker.js");
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
})
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
    console.log("worker on...");
})
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
worker.removeEventListener("alert");
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

##### 4.2.1.2.属性

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

#### 4.2.2.parentPort对象描述

worker线程用于与宿主线程通信的Object对象。

##### 4.2.2.1.接口

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

##### 4.2.2.2.属性

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

### 4.3涉及仓

- ace_ace_engine(foundation/arkui/ace_engine-readme_zh.md)
- ace_napi(foundation/arkui/napi-readme_zh.md)


### 相关仓

[ets_utils子系统](commonlibrary/ets_utils/README.md)

### 许可证

Util在[Mozilla许可证](https://www.mozilla.org/en-US/MPL/)下可用，说明文档详见[说明文档](https://gitee.com/openharmony/js_util_module/blob/master/mozilla_docs.txt)。有关完整的许可证文本，有关完整的许可证文本，请参见[许可证](https://gitee.com/openharmony-sig/commonlibrary_ets_utils/blob/master/LICENSE)
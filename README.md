# http_normalizer
###### http url normalizer for crawlers

This library is intented to produce normalized http url and not uri, for example http url dosn't have an userinfo part (https://tools.ietf.org/html/rfc7230).

#### structure of http urls

```
http-URI = "http:" "//" authority path-abempty [ "?" query ] [ "#" fragment ]
```

#### http_parts
No regex are used here, these function are intended to be used from a web-crawler, so they want reduce the size of string
and be as conservative as possible, if you use a parser to extract the arguments for these function be careful because parser
could be more restrictive. The advantage of having single functions that perform the normalization of the individual parts 
is the possibility of defining further parsing rules, for example if you want to decode the percent-encoded ports you can 
try this decoding of the sequence of characters involved before calling normalize_port () and therefore increase the general
error tolerance.

###### normalize_protocol()
* Decoding percent-encoded octets of unreserved characters
* Converting the scheme to lower case
* Then check if is an http/s scheme

return an empty string in case of error

###### normalize_hostname()
* Decoding percent-encoded octets of unreserved characters
* Empty labels are not allowed
* Max 64 chars are allowed
* Require at least 2 labels (so hosts like localhost are rejected)
* Maximum 20 labels
* Each labels must start and end with `[[:alnum:]]`, `'-'` is allowed in the middle
* Converting the host to lower case
* Remove all www. labels from the beginning of string (these are counted as part of  max_labels) so `www.www.google.com` become `google.com`

return an empty string in case of error

###### normalize_port()
* Check if the port is between (1-MAX_USHORT)
* If is a valid port and the port can be removed (is the default port of protocol, for example 443 for https) return 0, in case of error return -1
or the port itself if is valid and isn't the default port

###### normalize_path()
* Decoding percent-encoded octets of unreserved characters
* Encoding reserved characters (ex. `'\0'` become `%00`)
* Removing dot-segments (ex. `/../lol/` become `lol`)
* Removing double slashes (ex. `//lol/lol///` become `lol/lol`)
* Removing default directory index (ex. `/a/index.html`  become `a`)

This function always perform a normalization (no errors).

###### normalize_query()
* Decoding percent-encoded octets of unreserved characters
* Drop `'='` in Query Parameter if Empty (ex. `q=&x` become `q&x`)
* Encoding reserved characters
* Space becomes plus sign In query (ex. `q=%20` become `q=+`)
* Sorted query parameters by key (ex. `q=10&q=20&a=1` become `a=1&q=20`)

This function always perform a normalization (no errors).

###### Copyright © 2019, [Manu-sh](https://github.com/Manu-sh), s3gmentationfault@gmail.com. Released under the [MIT license](LICENSE).
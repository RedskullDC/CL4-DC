// The Swift Programming Language
// https://docs.swift.org/swift-book

import Foundation
import CL4

extension String {
    var UTF8CString: UnsafeMutablePointer<CChar>? {
        return UnsafeMutablePointer(mutating: (self as NSString).utf8String!)
    }
}

var ctx = CLinit("test.c".UTF8CString)
print (ctx)

if ctx > 0 {
    let str = String(cString: CLdbschema(ctx, "DEMGL".UTF8CString))
    print (str)

    let returnCode = CLshutdown(ctx)
    print (returnCode)
}


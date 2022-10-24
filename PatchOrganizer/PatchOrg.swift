//
//  PatchOrg.swift
//  PatchOrganizer
//
//  Created by Thibault DUCRAY on 19/10/2022.
//

import Foundation

var dummypatches: [UIPatch] = []
//    UIPatch(id: 0, bank:1, num:1, name: "Select file"),
//]


class PatchHandler {
    var patchList: PatchList
    var validFile: Bool
    var plp: UnsafeMutablePointer<PatchList>
    var initFilename: String
    var newFilename: String
    
    init(fileName: String) {
        self.initFilename = fileName
        self.newFilename = fileName
        plp = UnsafeMutablePointer<PatchList>.allocate(capacity: 1)
        //var cs = fileName.cString(using: String.Encoding.utf8)
        let cs = UnsafeMutablePointer<CChar>(mutating: fileName.cString(using: String.Encoding.utf8))
        let err = readPresetsFromFile(cs, plp)
        validFile = err == NBPATCHES
        patchList = plp.pointee
    }
    
    func getElem(i: Int) -> (bank: Int, num: Int, name: String) {
        let cs = UnsafeMutablePointer<CChar>.allocate(capacity: Int(PATCH_NAME_SIZE) + 1)
        getPatchNameForIndex(cs, plp, Int32(i))
        let name = String(cString: cs, encoding: String.Encoding.utf8) ?? "<noname>"
        cs.deallocate()
        
        let num = getPatchNumForIndex(plp, Int32(i))
        let b = Int(num / 3) + 1
        let n = Int(num) - (b-1) * 3 + 1
        return (bank: Int(b), num: Int(n), name: name)
    }
    
    func setElem(actualpos: Int, bank: Int, num: Int, name: String, newpos: Int) {
        //let actualpos = (bank - 1) * 3 + num - 1
        setPatchNumForIndex(plp, Int32(actualpos), UInt8(newpos))
        // set name one day ?
    }
    
    func writePatchlist(fileName: String) -> Int {
        self.newFilename = fileName
        let ocs = UnsafeMutablePointer<CChar>(mutating: self.initFilename.cString(using: String.Encoding.utf8))
        let ncs = UnsafeMutablePointer<CChar>(mutating: fileName.cString(using: String.Encoding.utf8))
        let ret = writePresetsToFile(ncs, ocs, plp)
        return ret
    }
    
    deinit {
        plp.deallocate()
    }
    
    
}

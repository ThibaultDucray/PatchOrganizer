//
//  PatchOrg.swift
//  PatchOrganizer
//
//  Created by Thibault DUCRAY on 19/10/2022.
//

// This file is part of PatchOrganizer.
//
// PatchOrganizer is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software Foundation,
// either version 3 of the License, or (at your option) any later version.
//
// PatchOrganizer is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
// without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
// PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with PatchOrganizer.
// If not, see <https://www.gnu.org/licenses/>.
//
// (c) Copyright 2022 Thibault Ducray

import Foundation



class PatchHandler {
    var patchList: PatchList
    var validFile: Bool
    var plp: UnsafeMutablePointer<PatchList>
    var actualFilename: String
    var newFilename: String
    var nbPatches: Int
    
    init(fileName: String) {
        self.actualFilename = fileName
        self.newFilename = fileName
        plp = UnsafeMutablePointer<PatchList>.allocate(capacity: 1)
        let err = readPresetsFromFile(actualFilename, plp)
        validFile = err > 0
        nbPatches = Int(err)
        patchList = plp.pointee
    }
    
    func getElem(i: Int) -> (bank: Int, num: Int, userIR: Bool, name: String) {
        let cs = UnsafeMutablePointer<CChar>.allocate(capacity: Int(PATCH_NAME_SIZE) + 1)
        getPatchNameForIndex(cs, plp, Int32(i))
        let name = String(cString: cs, encoding: String.Encoding.utf8) ?? "<noname>"
        cs.deallocate()
        
        let num = getPatchNumForIndex(plp, Int32(i))
        let b = Int(num / 3) + 1
        let n = Int(num) - (b-1) * 3 + 1
        let u = getUserIRForIndex(plp, Int32(i)) == 0 ? false : true
        return (bank: Int(b), num: Int(n), userIR: u, name: name)
    }
    
    func setElem(actualpos: Int, bank: Int, num: Int, name: String, newpos: Int) {
        setPatchNumForIndex(plp, Int32(actualpos), UInt8(newpos))
        // set name - might be hazardous
        setPatchNameForIndex(plp, Int32(actualpos), name)
    }
    
    func writePatchlist(fileName: String, invertTailBit: Bool) -> Int {
        self.newFilename = fileName
        let ret = writePresetsToFile(newFilename, actualFilename, plp, invertTailBit ? 1 : 0)
        return ret
    }
    
    deinit {
        plp.deallocate()
    }
    
    
}

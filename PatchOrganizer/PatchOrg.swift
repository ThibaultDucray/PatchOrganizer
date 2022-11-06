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
    var presets = PresetFile()
    var validFile: Bool
    var filedesc = Filedesc()
    var actualFilename: String
    var newFilename: String
    var nbPatches: Int
    
    // all allocations are now in charge of the caller, ie PatchHandler
    init(fileName: String) {
        self.actualFilename = fileName
        self.newFilename = fileName
        filedesc.size = fileSize(fileName);
        if (filedesc.size != 0) {
            filedesc.content = UnsafeMutablePointer<UInt8>.allocate(capacity: filedesc.size)
        }
        let err = createPresetsFromFile(&presets, &filedesc, fileName)
        //let err = readPresetsFromFile(actualFilename, &patchList, &filedesc)
        validFile = err > 0
        nbPatches = Int(err)
    }
    
    func getElem(i: Int) -> (bank: Int, num: Int, userIR: Bool, name: String) {
        let cs = UnsafeMutablePointer<CChar>.allocate(capacity: Int(PATCH_NAME_SIZE) + 1)
        getPatchNameForIndex(cs, &presets, Int32(i))
        let name = String(cString: cs, encoding: String.Encoding.utf8) ?? "<noname>"
        cs.deallocate()
        
        let num = getPatchNumForIndex(&presets, Int32(i))
        let b = Int(num / 3) + 1
        let n = Int(num) - (b-1) * 3 + 1
        let u = getUserIRForIndex(&presets, Int32(i)) == 0 ? false : true
        return (bank: Int(b), num: Int(n), userIR: u, name: name)
    }
    
    func exportOnePatch(fileName: String, index: Int, invertTailBit: Bool) -> Int {
        return writePresetsFileFromOnePatch(fileName, &presets, Int32(index), invertTailBit ? 1 : 0)
    }
    
    func setElem(actualpos: Int, name: String, newpos: Int) {
        setPatchNumForIndex(&presets, Int32(actualpos), UInt8(newpos))
        // set name - might be hazardous
        setPatchNameForIndex(&presets, Int32(actualpos), name)
    }
    
    func writePatchlist(fileName: String, invertTailBit: Bool) -> Int {
        self.newFilename = fileName
        let ret = writePresetsToFile(newFilename, &presets, invertTailBit ? 1 : 0)
        return ret
    }
    
    // warning with memory management... this is binded to C code that does not copy memory but reaffects pointers
    func replacePatch(source: PatchHandler, from: Int, to: Int) {
        exchangePatchesInPreset(&presets, Int32(to), &(source.presets), Int32(from))
    }
    
    deinit {
        if (filedesc.size != 0) {
            filedesc.content.deallocate()
        }
        freePresetFile(&presets)
    }
    
    
}

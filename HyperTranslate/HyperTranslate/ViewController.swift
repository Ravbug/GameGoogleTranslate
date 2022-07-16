//
//  ViewController.swift
//  HyperTranslate
//
//

import UIKit
import MLKitTranslate

class ViewController: UIViewController {
	@IBOutlet weak var InfoLabel: UILabel!
	@IBOutlet weak var ProgressBar: UIProgressView!
	
	let conditions = ModelDownloadConditions(
		allowsCellularAccess: false,
		allowsBackgroundDownloading: true
	)
	
	let languages : [MLKitTranslate.TranslateLanguage] = [
		.afrikaans,
		.albanian,
		.arabic,
		.belarusian,
		.bulgarian,
		.catalan,
		.chinese,
		.croatian,
		.czech,
		.danish,
		.dutch,
		.english,
		.estonian,
		.finnish,
		.french,
		.galician,
		.georgian,
		.german,
		.haitianCreole,
		.hebrew,
		.hindi,
		.hungarian,
		.icelandic,
		.indonesian,
		.irish,
		.italian,
		.japanese,
		.kannada,
		.korean,
		.latvian,
		.macedonian,
		.malay,
		.marathi,
		.norwegian,
		.persian,
		.polish,
		.portuguese,
		.romanian,
		.russian,
		.slovak,
		.slovenian,
		.spanish,
		.swahili,
		.swedish,
		.tagalog,
		.tamil,
		.thai,
		.turkish,
		.ukrainian,
		.urdu,
		.vietnamese,
		.welsh
	]
	
	func decideOrder(count : UInt8, completion: @escaping (Array<MLKitTranslate.TranslateLanguage>,Array<MLKitTranslate.Translator>) -> Void ){
		var sequence = Array<MLKitTranslate.TranslateLanguage>()
		var translators = Array<MLKitTranslate.Translator>()
		for _ in 0..<count-1{
			sequence.append(languages.randomElement()!)
		}
		sequence.append(.english) // and back to english
		// now download models
		InfoLabel.text = "Downloading Models"
		ProgressBar.progress = 0
		var beginLang = MLKitTranslate.TranslateLanguage.english
		var completedCount = 0
		for i in 0..<count{
			let targetLang = sequence[Int(i)]
			let options = TranslatorOptions(sourceLanguage: beginLang, targetLanguage: targetLang)
			let translator = Translator.translator(options: options)
			translators.append(translator)
			translator.downloadModelIfNeeded(with: conditions) { error in
				if error == nil {
					completedCount += 1
					self.ProgressBar.progress = Float(completedCount) / Float(count)
					if completedCount == count{
						completion(sequence,translators)
					}
				}
				else{
					fatalError("Could not download model: \(error!)")
				}
			}
			beginLang = targetLang	// for next loop iter
		}
	}
	
	func translateString(inputString : String, translators: Array<MLKitTranslate.Translator>, callback: @escaping (String) -> Void, index : Int = 0) -> Void{
		
		let translator = translators[index]
		translator.translate(inputString) { str, error in
			if (index == translators.count-1){
				callback(str!)
			}
			else{
				self.translateString(inputString: str!, translators: translators, callback: callback, index: index+1)
			}
		}
	}
	
	func translateAllStrings(inStrings : Array<String>, count : UInt8, callback : @escaping (Array<String>) -> Void){
		var strings = inStrings
		decideOrder(count: count) { sequence, translators in
			var completedCount = 0
			self.ProgressBar.progress = 0
			self.InfoLabel.text = "Translating Strings"
			for i in 0..<strings.count{
				self.translateString(inputString: strings[i], translators:translators, callback: {resultStr in
					completedCount += 1
					self.ProgressBar.progress = Float(completedCount) / Float(strings.count)
					strings[i] = resultStr
					if (completedCount == strings.count){
						callback(strings)
					}
				})
			}
		}
	}
	
	
	@IBAction func goPressed(_ sender: Any) {
		let documentsFolder = URL(string:NSSearchPathForDirectoriesInDomains(.documentDirectory, .userDomainMask, true).last!)!;
		let filePath = documentsFolder.appendingPathComponent("input.tsv")
		
		do {
			let urls = FileManager.default.urls(for: .documentDirectory, in: .userDomainMask)[0]
			let text = try String(contentsOf: urls.appendingPathComponent("input.tsv"), encoding: .utf8)
			let strings = text.components(separatedBy: "\r\n")
			translateAllStrings(inStrings: strings, count: 15, callback: {strings in
				// TODO: write to the file without making a new string
				var string = String()
				for var str in strings{
					string.append("\(str)\r\n")
				}
				let url = urls.appendingPathComponent("output.tsv")
				try! string.write(to: url,atomically: false,encoding: .utf8)
				self.InfoLabel.text = "Completed!"
			})
		}
		catch{
			print("No file at \(filePath.absoluteString) : \(error)")
		}
	}
	
	override func viewDidLoad() {
		super.viewDidLoad()
		// Do any additional setup after loading the view.
		
	}


}


import 'dart:io';
import 'package:firebase_storage/firebase_storage.dart';
import 'package:file_picker/file_picker.dart';
import 'package:image_picker/image_picker.dart';

class StorageService {
  final FirebaseStorage _storage = FirebaseStorage.instance;

  /// Upload an image using File Picker
  Future<String?> uploadFile() async {
    FilePickerResult? result = await FilePicker.platform.pickFiles();
    
    if (result != null) {
      File file = File(result.files.single.path!);
      String fileName = DateTime.now().millisecondsSinceEpoch.toString();
      Reference ref = _storage.ref().child("uploads/$fileName.jpg");
      
      await ref.putFile(file);
      return await ref.getDownloadURL();
    }
    return null;
  }

  /// Upload an image from Camera or Gallery
  Future<String?> uploadImage(ImageSource source) async {
    final ImagePicker _picker = ImagePicker();
    XFile? image = await _picker.pickImage(source: source);
    
    if (image != null) {
      File file = File(image.path);
      String fileName = DateTime.now().millisecondsSinceEpoch.toString();
      Reference ref = _storage.ref().child("uploads/$fileName.jpg");
      
      await ref.putFile(file);
      return await ref.getDownloadURL();
    }
    return null;
  }

  /// List all uploaded files
  Future<List<String>> listFiles() async {
    ListResult result = await _storage.ref("uploads").listAll();
    List<String> urls = [];

    for (var ref in result.items) {
      String url = await ref.getDownloadURL();
      urls.add(url);
    }
    return urls;
  }

  /// Delete a file from Firebase Storage
  Future<void> deleteFile(String fileUrl) async {
    try {
      Reference ref = _storage.refFromURL(fileUrl);
      await ref.delete();
    } catch (e) {
      print("Error deleting file: $e");
    }
  }
}
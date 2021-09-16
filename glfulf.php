<?php
//ini_set('max_execution_time', 2000);
header('Content-type: text/html; charset=utf-8');
// List of input orders
function listInputOrders()
{
  $fname="";
  foreach (glob("./input/*.rt") as $filename) {
    $fname.=substr(strrchr($filename,'/'),1)." ";
  }  
  return iconv('utf-8','utf-8',$fname);
}
// List of output orders
function listOutputOrders()
{
  $fname="";
  foreach (glob("./output/*.rt") as $filename) {
    $fname.=substr(strrchr($filename,'/'),1)." ";
  }  
  return iconv('utf-8','utf-8',$fname);
}

function getOutputOrder($order)
{
  $destination = realpath('./output');
  $fsel=iconv('utf-8','utf-8',$order);
  if(!strlen($fsel)) { // absent write name of selected file
    return -1 ; 
  }
  // find ID index of selected file if don't found return 0
  if(file_exists($destination."/".$fsel)){ 
    return file_get_contents($destination."/".$fsel);
  }

}
function putInputOrder()
{
  $destination = realpath('./input');
  $file = $_FILES["uploaded"];
  $fn=iconv('utf-8','utf-8',$file["name"]);
  $filename = $destination."/".preg_replace("|[\\\/]|", "", $fn);
//  $filename = $destination."/".$fn;
 // $sname = md5($file["name"]);

  //check that file name is valid
  if ($filename !== ""){
    move_uploaded_file($file["tmp_name"], $filename);
    return "0" ;
  }

  return "-1";
}

function removeInputOrder($order)
{
  $destination = realpath('./input');
  $fsel=iconv('utf-8','utf-8',$order);
  if(!strlen($fsel)) {
    return "-1"; 
  } 
  if(!file_exists($destination."/".$fsel)) {
    return "-2";
  }
  unlink($destination."/".$fsel); 
  return "0";
}

function removeOutputOrder($order)
{
  $destination = realpath('./output');
  $fsel=iconv('utf-8','utf-8',$order);
  if(!strlen($fsel)) {
    return "-1"; 
  } 
  if(!file_exists($destination."/".$fsel)) {
    return "-2";
  }
  unlink($destination."/".$fsel); 
  return "0";
}

// main

 if(isset($_POST["list_input"])){
   echo listInputOrders();
 }
 else if(isset($_POST["list_output"])){
   echo listOutputOrders();
 }
 else if(isset($_POST["remove_input"])){
   echo removeInputOrder($_POST["remove_input"]);
 }
 else if(isset($_POST["remove_output"])){
   echo removeOutputOrder($_POST["remove_output"]);
 }
 else if(isset($_POST["get_output"])){
   echo getOutputOrder($_POST["get_output"]);
 }
 else if(isset($_FILES["uploaded"])){
   echo putInputOrder();
 }
//else 
//echo putInputOrder($_POST["put_input"],$_POST["input_data"]);
 ?>
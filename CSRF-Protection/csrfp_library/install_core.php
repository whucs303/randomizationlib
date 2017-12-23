<?php
/*the class for implementing the randomization protection to the web site 
in order to against the CSRF Attack. 
Author:ben 
Date:2015/03/18
*/
class Implementation
{
	//store the dir of php programs
	public static $_directory="";        #the path of the directories to be patched
	public static $_files="";            #the files to be patched
	public static $_para_name=array();   #store the paranames of tha whole htmls
    public static $_final_name=array();  #no-repeatedly paranames
    public static $para_count=0;         #count the number of pranames
	//imlement the protection
	public static function implement()
	{
	    self::Getdir();          #get the path of the directories and files to be patched from the config file
        self::handlefiles();     #patch the html and the php
        self::generate_globalparameter_php();  #generate the php script for assignment of rand_paranames
       // self::copy_file();
	}
	//get the configured information from config.ini
	public static function Getdir()
	{
	    $configfile="config.ini";
	    if(!file_exists($configfile))
		{
		    die("The configuration file does not exists!"); 
		}
		$dir=parse_ini_file($configfile,true);
		self::$_directory=$dir['sitedir'];
	}
	
	//handle the files needed to be patched
	public static function handlefiles()
	{
	    foreach(self::$_directory as $key=>$value)
		{
		    if($value!=""&&is_dir($value))
			{
			    //traverse the dir
                self::traversedir($value);
			}
			else
			{
			    die("Please configure the config.ini first!");   
			}
		}
	}
	
	//tranverse the dir
	public static function traversedir($dir)
	{
	    $subdir=scandir($dir);
		foreach($subdir as $key=>$value)
		{
		    if($value!="."&&$value!="..")
			{
			    $path=$dir."/".$value;
				if(is_dir($path))
				{   //recurse the subdir
				    self::traversedir($path);
				}
				else
				{   //patch the file
				    $postfix=strrchr($path,'.');
					switch($postfix)
					{   //patch the two different files
					    case ".php":
						    self::patchphpfile($path);  //patch the POST[xxx]
							break;
						case ".htm":
						    self::patchhtmlfile($path);  //patch the html template
						    //collect the related parameter names
                            foreach(self::$_para_name as $key=>$value)self::$_final_name[self::$para_count++]=$value;
						    break;
						default:
						    continue;
					}    
				}
			}
		}
	}
	
	/*patch the php file with csrf randomization protection
	  replace all the http request keyword:$_POST[]
	  with the packaged methods
    */
	public static function patchphpfile($path)
	{
	    $pattern='/\$_POST\[.+?\]/';
        $myfile=fopen($path,'r+');
        $file_text=fread($myfile,filesize($path));
        if(preg_match_all($pattern,$file_text,$match))
        { 
			echo "PHP Patching: ".$path.'<br>';
			$pattern1='/(\$_POST)(\[)(.+?)(\])/';
			$file_text=preg_replace($pattern1,"\${1}[CSRFP_Randomization::Randomization(\${3})]",$file_text);
			rewind($myfile);
			fwrite($myfile,$file_text);
			fclose($myfile);
        }
        else
        {
			echo "Not found in: ".$path.'<br>';
        }
	}
	
	/*patch the html file with csrf randomization protection
	  replace the value of parameter name with randomized value
	*/
 	 public static function patchhtmlfile($path)
    {
        $pattern='/<(?:input|textarea|select).*name=\"([A-Za-z0-9_]+?)\".*?>/';
        $myfile=fopen($path,"r+");
        $file_text=fread($myfile,filesize($path));
        if(preg_match_all($pattern,$file_text,$match))
        {
            echo "HTML Patching: ".$path.'<br>';
            self::$_para_name=$match[1];
            print_r(self::$_para_name);
            echo '<br>';
            $pattern1="/(<(?:input|textarea|select).*name=)(\")([A-Za-z0-9_]+?\".*?>)/";
            //Tips:developers may be required to modify this code according to  specific template engine
            $file_replace=preg_replace($pattern1,"\${1}\"\$rand_\${3}",$file_text);
            rewind($myfile);
            fwrite($myfile,$file_replace);
            fclose($myfile);  
        }
        else
        {
             echo "Not found in: ".$path.'<br>';
        }   
    }

/*
the following functions are used for testing on DISCUZ 3.X
*/
    /*generate a script for assignment of rand_parameters*/
    public static function generate_globalparameter_php()
    {
        if(!empty(self::$_final_name))
        {
            $temp_script='<?php';
            $temp_script.="\r\nrequire_once dirname(__FILE__).'/csrfp_randomization.php';\r\nCSRFP_Randomization::init();";
            $unique_name=array_unique(self::$_final_name);
            foreach($unique_name as $key=>$value)$temp_script.="\r\n".'$rand_'.$value.'='.'CSRFP_Randomization::Randomization(\''.$value.'\');';
            $temp_script.="\r\n?>";
            $myscript=fopen('csrfp_paraname_define.php','w');
            fwrite($myscript,$temp_script);
            fclose($myscript);
        }
    }
    /*patching :add include file to the header of each php script*/
	public static function patch_include($path)
	{
	    $add_string="require_once './";
	    $count_path=substr_count($path,"/");
	    echo $count_path;
	    $add_string.=str_repeat("../", $count_path-5);        	
	    $add_string.="source/class/csrfp/csrfp_paraname_define.php';\r\n";
	    $lines=file($path);
	    $myfile=fopen($path,'w');
	    if(!$myfile)echo "can not open the file";
	    echo "patching file:".$path."<br>";
	    foreach($lines as $line_num=>$line)
	    {
	        if($line_num==1)fwrite($myfile,$add_string);
	        fwrite($myfile,$line);
	    }
	    fclose($myfile);
	}

	/*copy the core files to discuz directory*/
	public static function copy_file()
	{
	    $dir_path=self::$_site_dir['site_dir']."/source/class/csrfp";
	    if(!mkdir($dir_path))
	    {
	    	die("mkdir failed at /source/class/csrfp");
	    }
	    if(!copy(dirname(__FILE__)."/csrfp_randomization.php",$dir_path."/csrfp_randomization.php"))
	    {
	    	die("copying failed at csrfp_randomization.php");
	    }
	    if(!copy(dirname(__FILE__)."/csrfp_paraname_define.php",$dir_path."/csrfp_paraname_define.php"))
	    {
	    	die("copying failed at csrfp_paraname_define.php");
	    }	
	}
}
?>

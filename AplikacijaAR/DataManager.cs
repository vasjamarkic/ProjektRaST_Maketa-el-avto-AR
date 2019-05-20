using System.Collections;
using System.Collections.Generic;
using UnityEngine.Networking;
using UnityEngine;
using TMPro;

public class eleData {
    public float TempController;
    public float TempBattery;
    public float TempMotor;
    public float TempBMS;
    public float BatVoltage;
    public bool fan;
    public bool BMS_start;
    public float AkuVoltage;
    public int time;

}


public class DataManager : MonoBehaviour
{
    [Header("Addresses")]
    public string statusAdd;
    public string logAdd;

    [Header("UI_holders")]
    public GameObject[] instructions;
    public GameObject[] tips;
    public GameObject[] information;

    [Header("TextViews")]
    public TextMeshProUGUI  tempController;
    public TextMeshProUGUI  tempBattery;
    public TextMeshProUGUI  tempMotor;
    public TextMeshProUGUI  tempBMS ;
    public TextMeshProUGUI  BatVoltage;
    public TextMeshProUGUI  fan;
    public TextMeshProUGUI  BMS_start;
    public TextMeshProUGUI  AkuVoltage;

    [Header("Properties")]
    public float dataUpdateTime;
    public float[] temps;
    public Color32[] colors;

    [Header("Colors")]
    public Color32 onColor;
    public Color32 offColor;

    private bool tutorialInProgress;
    // Start is called before the first frame update
    void Start()
    {
        StartCoroutine(dataUpdate());
        HideAll();
        beginGuide();
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    void HideAll(){
        foreach(GameObject g in instructions){
            g.SetActive(false);
        }
        foreach(GameObject g in tips){
            g.SetActive(false);
        }
        foreach(GameObject g in information){
            g.SetActive(false);
        }
    }

    public void beginGuide(){
        Debug.Log("Guide begins");
        HideAll();
        instructions[0].SetActive(true);
        instructions[1].SetActive(true);
        tutorialInProgress = true;
    }

    public void ShowInstruction(int id){
        HideAll();
        instructions[id].SetActive(true);
    }

    public void Showdata(){
        HideAll();
        foreach(GameObject g in information){
            g.SetActive(true);
        }
    }

    IEnumerator dataUpdate() {

        StartCoroutine(refreshData());
        yield return new WaitForSeconds(dataUpdateTime);
        StartCoroutine(dataUpdate());

    }

    void assignColor(TextMeshProUGUI txt, float data) {

        for (int i = 0; i < temps.Length; i++)
        {
            if (data < temps[i]) {
                txt.color = colors[i];
                return;
            }
        }
        txt.color = colors[temps.Length -1];
        return;

    }

    void ProcessData(string jsonStr) {

        eleData data = JsonUtility.FromJson<eleData>(jsonStr);

        print(data);

        assignColor(tempController, data.TempController);
        tempController.text = data.TempController+ " °C";
        assignColor(tempBattery, data.TempBattery);
        tempBattery.text = data.TempBattery + " °C";
        assignColor(tempMotor, data.TempMotor);
        tempMotor.text = data.TempMotor + " °C";
        assignColor(tempBMS, data.TempBMS);
        tempBMS.text = data.TempBMS + " °C";

        BatVoltage.text = data.BatVoltage+" V";
        
        AkuVoltage.text = data.AkuVoltage+" V";

        if (data.BMS_start)
        {
            BMS_start.text = "Aktiven";
            BMS_start.color = onColor;

            if(tutorialInProgress){
                ShowInstruction(4);
                tutorialInProgress = false; 
                }
        }
        else
        {
            BMS_start.text = "Neaktiven";
            BMS_start.color = offColor;
        }

    }

    IEnumerator refreshData() {

        using (UnityWebRequest req = UnityWebRequest.Get(statusAdd)) {
            yield return req.SendWebRequest();
            Debug.LogAssertion(req.downloadHandler.text);
            ProcessData(req.downloadHandler.text);
        }
    }
}

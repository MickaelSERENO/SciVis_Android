package com.sereno.vfv;

import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentManager;
import android.support.v4.app.FragmentPagerAdapter;

import java.util.ArrayList;
import java.util.List;

class ViewPagerAdapter extends FragmentPagerAdapter
{
    private final List<Fragment> m_fragmentList      = new ArrayList<>();
    private final List<String>   m_fragmentTitleList = new ArrayList<>();

    public ViewPagerAdapter(FragmentManager manager)
    {
        super(manager);
    }

    @Override
    public Fragment getItem(int position)
    {
        return m_fragmentList.get(position);
    }

    @Override
    public int getCount()
    {
        return m_fragmentList.size();
    }

    public void addFragment(Fragment fragment, String title)
    {
        m_fragmentList.add(fragment);
        m_fragmentTitleList.add(title);
    }

    @Override
    public CharSequence getPageTitle(int position)
    {
        return m_fragmentTitleList.get(position);
    }
}